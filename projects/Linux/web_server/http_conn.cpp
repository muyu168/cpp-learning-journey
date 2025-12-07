
#include "locker.h"
#include "http_conn.h"

int http_conn::m_epollfd = -1;
int http_conn::m_user_count = 0;

// 定义HTTP响应的一些状态信息
const char* ok_200_title = "OK";
const char* error_400_title = "Bad Request";
const char* error_400_form = "Your request has bad syntax or is inherently impossible to satisfy.\n";
const char* error_403_title = "Forbidden";
const char* error_403_form = "You do not have permission to get file from this server.\n";
const char* error_404_title = "Not Found";
const char* error_404_form = "The requested file was not found on this server.\n";
const char* error_500_title = "Internal Error";
const char* error_500_form = "There was an unusual problem serving the requested file.\n";

const char* doc_root = "/home/wzh/cpp-learning-journey/projects/Linux/web_server/resources";

//设置文件描述符非阻塞 
void setnonblocking(int fd){
    int old_flag = fcntl(fd, F_GETFL);
    int new_flag = old_flag | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_flag);
}

//向epoll中添加需要监听的文件描述符
void addfd(int epollfd,int fd , bool one_shot){
    epoll_event event;
    event.data.fd = fd;
    //event.events = EPOLLIN | EPOLLRDHUP;
    event.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
    if(one_shot){
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
    //设置文件描述符非阻塞o
    setnonblocking(fd);
}

//从epoll中移除监听的文件描述符
void removefd(int epollfd, int fd){
    epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,0);
    close(fd);
}

//修改文件描述符，重置socket上EPOLLONESHOT事件，以确保下一次可读时，EPOLLIN事件能被触发
void modfd(int epollfd, int fd, int ev){

    epoll_event event;
    event.data.fd = fd;
    event.events = ev | EPOLLONESHOT | EPOLLRDHUP | EPOLLET;
    epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&event);
}

void http_conn::init(){
    m_check_state = CHECK_STATE_REQUESTLINE;//初始化状态为解析请求首行
    m_checked_index = 0;
    m_start_line = 0;
    m_read_idx = 0;
    m_write_idx = 0;
    m_method = GET;
    m_url = NULL;
    m_version = NULL;
    m_host = NULL;
    m_linger = false;
    m_content_length = 0;
    m_file_address = NULL;
    memset(m_read_buf, '\0', READ_BUFFER_SIZE);
    memset(m_write_buf, '\0', WRITE_BUFFER_SIZE);
    memset(m_real_file, '\0', FILENAME_LEN);
}

//初始化连接
void http_conn::init(int sockfd, const sockaddr_in & addr){
    m_sockfd = sockfd;
    m_address = addr;

    //端口复用
    int reuse = 1;
    setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    //添加到epoll对象中
    addfd(m_epollfd, m_sockfd, true);
    m_user_count++;// 总用户数加一

    init();
}


//关闭连接
void http_conn::close_conn(){
    if(m_sockfd != -1){
        removefd(m_epollfd, m_sockfd);
        m_sockfd = -1;
        m_user_count--;
    }
}

//读操作
bool http_conn::read(){
    if(m_read_idx >= READ_BUFFER_SIZE){
        return false;
    }

    //读取到的字节
    int bytes_read = 0;
    while(1){
        bytes_read = recv(m_sockfd,m_read_buf + m_read_idx, READ_BUFFER_SIZE - m_read_idx,0);
        if(bytes_read == -1){
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                //没有数据
                break;
            }
            return false;
        }else if(bytes_read == 0){
            //对方关闭连接
            return false;
        }
        m_read_idx += bytes_read;
    }
    // 使用%.*s安全打印，避免乱码
    printf("读到了数据：%.*s\n", m_read_idx, m_read_buf);
    return true;
}

// 写HTTP响应
bool http_conn::write()
{
    int temp = 0;
    int bytes_have_send = 0;    // 已经发送的字节
    // 计算总字节数：写缓冲区 + 文件内容（如果有）
    int bytes_to_send = m_write_idx;
    if ( m_iv_count == 2 ) {
        bytes_to_send += m_file_stat.st_size;
    }
    
    if ( bytes_to_send == 0 ) {
        // 将要发送的字节为0，这一次响应结束。
        modfd( m_epollfd, m_sockfd, EPOLLIN ); 
        init();
        return true;
    }

    while(1) {
        // 分散写
        temp = writev(m_sockfd, m_iv, m_iv_count);
        printf("writev 返回: %d, bytes_have_send: %d, bytes_to_send: %d\n", temp, bytes_have_send, bytes_to_send);
        if ( temp <= -1 ) {
            // 如果TCP写缓冲没有空间，则等待下一轮EPOLLOUT事件，虽然在此期间，
            // 服务器无法立即接收到同一客户的下一个请求，但可以保证连接的完整性。
            if( errno == EAGAIN ) {
                modfd( m_epollfd, m_sockfd, EPOLLOUT );
                return true;
            }
            unmap();
            return false;
        }
        bytes_have_send += temp;
        
        // 更新iovec结构，处理部分发送的情况
        if ( bytes_have_send >= m_write_idx ) {
            // 写缓冲区已经发送完
            if ( m_iv_count == 2 ) {
                m_iv[0].iov_len = 0;
                m_iv[1].iov_base = m_file_address + (bytes_have_send - m_write_idx);
                m_iv[1].iov_len = m_file_stat.st_size - (bytes_have_send - m_write_idx);
            }
        } else {
            // 写缓冲区还没发送完
            m_iv[0].iov_base = m_write_buf + bytes_have_send;
            m_iv[0].iov_len = m_write_idx - bytes_have_send;
        }
        
        if ( bytes_have_send >= bytes_to_send ) {
            // 发送HTTP响应成功，根据HTTP请求中的Connection字段决定是否立即关闭连接
            printf("响应发送完成，总字节数: %d\n", bytes_have_send);
            unmap();
            if(m_linger) {
                init();
                modfd( m_epollfd, m_sockfd, EPOLLIN );
                return true;
            } else {
                modfd( m_epollfd, m_sockfd, EPOLLIN );
                return false;
            } 
        }
    }
}


        //解析请求首行
http_conn::HTTP_CODE http_conn::parse_request_line(char *text){
    //找到请求方法
    m_url = strpbrk(text, " \t");
    //将m_url向后偏移一个位置（因为m_url指向的是第一个空格或\t字符，我们需要跳过它）
    *m_url++ = '\0';
    char *method = text;
    if(strcasecmp(method, "GET") == 0){
        m_method = GET;
    }else if(strcasecmp(method, "POST") == 0){
        m_method = POST;
    }
    //找到请求的版本号
    m_version = strpbrk(m_url, " \t");
    *m_version++ = '\0';
    if(strcasecmp(m_version, "HTTP/1.1") != 0){
        return BAD_REQUEST;
    }
    //检查url是否合法 - 处理绝对URL（如 http://host/path）
    if(strncasecmp("http://", m_url, 7) == 0){
        m_url += 7;
        m_url = strchr(m_url,'/');
        if(!m_url){
            // 如果URL是 http://host 形式，没有路径，说明格式有问题
            return BAD_REQUEST;
        }
    }
    //检查url是否以/开头
    if(!m_url || m_url[0] != '/'){
        return BAD_REQUEST;
    }

    printf("解析到的URL: %s\n", m_url);
    m_check_state = CHECK_STATE_HEADER; // 转移状态机到请求头状态
    
    return NO_REQUEST;
}
        //解析请求头
http_conn::HTTP_CODE http_conn::parse_headers(char *text){
    //遇到空行，表示请求头解析完毕
    if(text[0] == '\0'){
        if(m_content_length != 0){
            m_check_state = CHECK_STATE_CONTENT;
            return NO_REQUEST;
        }
        //否则说明我们已经得到了完整的HTTP请求
        return GET_REQUEST;
    }else if(strncasecmp(text , "Connection:", 11 ) == 0){
        //处理Connection头部字段 可能是keep-alive或close
        text += 11;
        text += strspn(text, " \t");
        if(strcasecmp(text, "keep-alive") == 0){
            m_linger = true;
        }
    }else if(strncasecmp(text , "Content-Length:", 15 ) == 0){
        //处理Content-Length头部字段
        text += 15;
        text += strspn(text, " \t");
        m_content_length = atoi(text);
    }else if(strncasecmp(text , "Host:", 5 ) == 0){
        //处理Host头部字段
        text += 5;
        text += strspn(text, " \t");
        m_host = text;
    }else{
        // 其他未知头部字段，忽略而不是返回错误（符合HTTP标准）
        printf("未知头部字段（已忽略）: %s\n", text);
    }
    return NO_REQUEST;
}
        //解析请求体
http_conn::HTTP_CODE http_conn::parse_content(char *text){
    if(m_read_idx >= (m_content_length + m_checked_index)){
        // 请求体已完整读取，不需要修改缓冲区
        m_checked_index += m_content_length;
        return GET_REQUEST;
    }
    return NO_REQUEST;
}
//解析一行，判断依据\r\n
http_conn::LINE_STATUS http_conn::parse_line(){
    char temp;
    for( ; m_checked_index < m_read_idx; ++m_checked_index){
        temp = m_read_buf[m_checked_index];
        if(temp == '\r'){
            if((m_checked_index + 1) == m_read_idx){
                return LINE_OPEN;
            }else if(m_read_buf[m_checked_index + 1] == '\n'){
                m_read_buf[m_checked_index++] = '\0';
                m_read_buf[m_checked_index++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }else if(temp == '\n'){
            if((m_checked_index > 1) && m_read_buf[m_checked_index - 1] == '\r'){
                m_read_buf[m_checked_index - 1] = '\0';
                m_read_buf[m_checked_index++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
    }
    return LINE_OPEN;
}

http_conn::HTTP_CODE http_conn::do_request()
{
    
    strcpy( m_real_file, doc_root );
    int len = strlen( doc_root );
    
    // 如果URL是"/"或空，默认返回index.html
    if( !m_url || strcmp( m_url, "/" ) == 0 ) {
        strncpy( m_real_file + len, "/index.html", FILENAME_LEN - len - 1 );
    } else {
        strncpy( m_real_file + len, m_url, FILENAME_LEN - len - 1 );
    }
    m_real_file[FILENAME_LEN - 1] = '\0';  // 确保字符串结束
    
    printf("请求的文件路径: %s (URL: %s)\n", m_real_file, m_url ? m_url : "NULL");
    
    // 获取m_real_file文件的相关的状态信息，-1失败，0成功
    if ( stat( m_real_file, &m_file_stat ) < 0 ) {
        printf("文件不存在或无法访问: %s, errno: %d\n", m_real_file, errno);
        return NO_RESOURCE;
    }

    // 判断访问权限
    if ( ! ( m_file_stat.st_mode & S_IROTH ) ) {
        return FORBIDDEN_REQUEST;
    }

    // 判断是否是目录
    if ( S_ISDIR( m_file_stat.st_mode ) ) {
        return BAD_REQUEST;
    }

    // 以只读方式打开文件
    int fd = open( m_real_file, O_RDONLY );
    if ( fd < 0 ) {
        printf("打开文件失败: %s, errno: %d\n", m_real_file, errno);
        return NO_RESOURCE;
    }
    
    // 创建内存映射
    m_file_address = ( char* )mmap( 0, m_file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0 );
    close( fd );
    
    if ( m_file_address == MAP_FAILED ) {
        printf("内存映射失败: %s, errno: %d\n", m_real_file, errno);
        return INTERNAL_ERROR;
    }
    
    printf("文件映射成功，大小: %ld 字节\n", m_file_stat.st_size);
    return FILE_REQUEST;
}

// 对内存映射区执行munmap操作
void http_conn::unmap() {
    if( m_file_address )
    {
        munmap( m_file_address, m_file_stat.st_size );
        m_file_address = 0;
    }
}
//主状态机,解析请求

http_conn::HTTP_CODE http_conn::process_read(){
    LINE_STATUS line_status = LINE_OK;
    HTTP_CODE ret = NO_REQUEST;

    char *text = 0;
    
    printf("process_read 开始，m_check_state: %d, m_read_idx: %d, m_checked_index: %d\n", 
           m_check_state, m_read_idx, m_checked_index);

    while ( ((m_check_state == CHECK_STATE_CONTENT) && (m_read_idx >= (m_content_length + m_checked_index)))
         || ((line_status = parse_line()) == LINE_OK) ){
        //解析到了一行完整数据，或者解析到了请求体
        if ( m_check_state == CHECK_STATE_CONTENT ) {
            // 处理请求体，不需要按行解析
            text = m_read_buf + m_checked_index;
            ret = parse_content(text);
            if(ret == GET_REQUEST){
                return do_request();
            }
            line_status = LINE_OPEN;
        } else {
            // 处理请求行和请求头
            text = get_line();
            m_start_line = m_checked_index;
            printf("got 1 http line : %s\n",text);
            switch(m_check_state){
                case CHECK_STATE_REQUESTLINE:
                {
                    ret = parse_request_line(text);
                    if(ret == BAD_REQUEST){
                        return BAD_REQUEST;
                    }
                    break;
                }
                case CHECK_STATE_HEADER:
                {
                    ret = parse_headers(text);
                    if(ret == BAD_REQUEST){
                        return BAD_REQUEST;
                    }else if(ret == GET_REQUEST){
                        return do_request();
                    }
                    break;
                }
                default:
                    return INTERNAL_ERROR;
            }
        }

    }
    printf("process_read 循环结束，返回 NO_REQUEST, line_status: %d\n", line_status);
    return NO_REQUEST;
}
// 根据服务器处理HTTP请求的结果，决定返回给客户端的内容
bool http_conn::process_write(HTTP_CODE ret) {
    switch (ret)
    {
        case INTERNAL_ERROR:
            add_status_line( 500, error_500_title );
            add_headers( strlen( error_500_form ) );
            if ( ! add_content( error_500_form ) ) {
                return false;
            }
            break;
        case BAD_REQUEST:
            add_status_line( 400, error_400_title );
            add_headers( strlen( error_400_form ) );
            if ( ! add_content( error_400_form ) ) {
                return false;
            }
            break;
        case NO_RESOURCE:
            add_status_line( 404, error_404_title );
            add_headers( strlen( error_404_form ) );
            if ( ! add_content( error_404_form ) ) {
                return false;
            }
            break;
        case FORBIDDEN_REQUEST:
            add_status_line( 403, error_403_title );
            add_headers(strlen( error_403_form));
            if ( ! add_content( error_403_form ) ) {
                return false;
            }
            break;
        case FILE_REQUEST:
            add_status_line(200, ok_200_title );
            add_headers(m_file_stat.st_size);
            m_iv[ 0 ].iov_base = m_write_buf;
            m_iv[ 0 ].iov_len = m_write_idx;
            m_iv[ 1 ].iov_base = m_file_address;
            m_iv[ 1 ].iov_len = m_file_stat.st_size;
            m_iv_count = 2;
            printf("准备发送文件，响应头大小: %d, 文件大小: %ld\n", m_write_idx, m_file_stat.st_size);
            printf("响应头内容: %.*s\n", m_write_idx, m_write_buf);
            return true;
        default:
            return false;
    }

    m_iv[ 0 ].iov_base = m_write_buf;
    m_iv[ 0 ].iov_len = m_write_idx;
    m_iv_count = 1;
    return true;
}

// 往写缓冲中写入待发送的数据
bool http_conn::add_response( const char* format, ... ) {
    if( m_write_idx >= WRITE_BUFFER_SIZE ) {
        return false;
    }
    va_list arg_list;
    va_start( arg_list, format );
    int len = vsnprintf( m_write_buf + m_write_idx, WRITE_BUFFER_SIZE - 1 - m_write_idx, format, arg_list );
    if( len >= ( WRITE_BUFFER_SIZE - 1 - m_write_idx ) ) {
        return false;
    }
    m_write_idx += len;
    va_end( arg_list );
    return true;
}

bool http_conn::add_status_line( int status, const char* title ) {
    return add_response( "%s %d %s\r\n", "HTTP/1.1", status, title );
}

bool http_conn::add_headers(int content_len) {
    add_content_length(content_len);
    add_content_type();
    add_linger();
    add_blank_line();
    return true;
}

bool http_conn::add_content_length(int content_len) {
    return add_response( "Content-Length: %d\r\n", content_len );
}

bool http_conn::add_linger()
{
    return add_response( "Connection: %s\r\n", ( m_linger == true ) ? "keep-alive" : "close" );
}

bool http_conn::add_blank_line()
{
    return add_response( "%s", "\r\n" );
}

bool http_conn::add_content( const char* content )
{
    return add_response( "%s", content );
}

bool http_conn::add_content_type() {
    // 根据文件扩展名判断Content-Type，这里简化处理，统一返回text/html
    return add_response("Content-Type: %s; charset=utf-8\r\n", "text/html");
}

//由线程池中的工作线程调用，这是处理HTTP请求的入口函数
void http_conn::process(){
    printf("========== process() 被调用 ==========\n");
    printf("m_read_idx: %d, m_checked_index: %d\n", m_read_idx, m_checked_index);
    printf("读取缓冲区内容: %.*s\n", m_read_idx, m_read_buf);

    //解析HTTP请求
    HTTP_CODE read_ret = process_read();
    printf("process_read 返回: %d\n", read_ret);
    if(read_ret == NO_REQUEST){
        modfd(m_epollfd, m_sockfd, EPOLLIN);
        return;
    }

    //生产响应
    bool write_ret = process_write(read_ret);
    printf("process_write 返回: %d\n", write_ret);
    if(!write_ret){
        printf("process_write 失败，关闭连接\n");
        close_conn();
        return;
    }
    modfd(m_epollfd, m_sockfd, EPOLLOUT);
    printf("已注册 EPOLLOUT 事件，准备发送响应\n");

}


