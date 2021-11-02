//
// Created by vurdolag on 31.10.2021.
//

#include "RequestSession.h"


size_t write_callback(char * cont_buff, size_t size, size_t n, std::string * buff) {
    size_t s = size * n;
    if (buff) {
        buff->append(cont_buff, s);
    }
    return s;
}


void RequestSession::set_default_opt() {
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS , 10);
    //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
    curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, 30);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip,deflate,br");

    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1);

    // debug info
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

    if (user_agent) {
        curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent);
    }
}

void RequestSession::set_opt(std::string * buff, const char * url, bool is_post,
                             const char * params, const char * data) {

    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookie_file_name);
    curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookie_file_name);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buff);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    if (is_post) {
        curl_easy_setopt(curl, CURLOPT_POST, 1);
    }

    if (is_post && data) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    }
}

void RequestSession::set_headers(bool is_post) {
    //curl_slist * headers = nullptr;
    //curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
}

void RequestSession::set_proxy() {
    if (proxy) {
        curl_easy_setopt(curl, CURLOPT_PROXY, proxy->url.c_str());
        curl_easy_setopt(curl, CURLOPT_PROXYUSERNAME, proxy->user.c_str());
        curl_easy_setopt(curl, CURLOPT_PROXYPASSWORD, proxy->password.c_str());
    }
}

int RequestSession::request() {
    int res = curl_easy_perform(curl);

    curl_slist * cookies = nullptr;
    curl_slist * each;

    auto r = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
    if(!r && cookies) {
        /* a linked list of cookies in cookie file format */
        each = cookies;
        while(each) {
            std::cout << "cookie: " << each->data << std::endl;
            each = each->next;
        }
        curl_slist_free_all(cookies);
    }

    return res;
}

int RequestSession::POST(const char * url, const char * params, const char * data, std::string * buff) {
    set_opt(buff, url, true, params, data);
    set_headers(true);
    return request();
}

int RequestSession::GET(const char * url, const char * params, std::string * buff) {
    set_opt(buff, url, false, params, nullptr);
    set_headers(false);
    return request();
}

RequestSession::RequestSession(const char * name, const char * user_agent_, Proxy * proxy_)
              : cookie_file_name(name), user_agent(user_agent_), proxy(proxy_) {
    curl = curl_easy_init();
    set_default_opt();
    set_proxy();
}

RequestSession::~RequestSession() {
    curl_easy_cleanup(curl);
}

