#include <iostream>
#include <string>
#include <mutex>
#include <cstdlib>
#include <httplib.h>
#include <hiredis/hiredis.h>

std::mutex redis_mutex;

int main() {
    const char* redis_host = std::getenv("REDIS_HOST") ? std::getenv("REDIS_HOST") : "127.0.0.1";
    int redis_port = 6379;

    std::cout << "Svurzvane s Redis na adres: " << redis_host << "..." << std::endl;
    redisContext* context = redisConnect(redis_host, redis_port);
    
    if (context == NULL || context->err) {
        std::cerr << "Greshka pri vruzka s Redis!" << std::endl;
        if (context) redisFree(context);
        return 1;
    }
    std::cout << "Uspeshna vruzka s Redis database!" << std::endl;

    httplib::Server svr;

    svr.Get("/", [context](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(redis_mutex);
        
        redisReply* reply = (redisReply*)redisCommand(context, "INCR hits");
        long long hits = 0;
        if (reply) {
            hits = reply->integer;
            freeReplyObject(reply);
        }

        std::string html = "<html><head><title>C++ Docker App</title></head>";
        html += "<body style='font-family: Arial; text-align: center; margin-top: 50px;'>";
        html += "<h1>Zadanie: Konteinerizacia s Docker Compose</h1>";
        html += "<p>Tozi C++ survur raboti v Docker konteiner.</p>";
        html += "<h2>Obshto poseshtenia: <span style='color: green;'>" + std::to_string(hits) + "</span></h2>";
        html += "</body></html>";
        
        res.set_content(html, "text/html");
    });

    std::cout << "Serverut startira na port 8080..." << std::endl;
    svr.listen("0.0.0.0", 8080);

    redisFree(context);
    return 0;
}