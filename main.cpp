#include <iostream>
#include"RateLimiter.cpp"

int main() {
    RateLimiter limiter(5,1);
    std::string userId="mourad-123";
    for (int i=1;i<=10;i++) {
        RateLimitResult result=limiter.allowRequest(userId);
        if (result.allowed) {
            std::cout << "Request " << i << " allowed. Remaining tokens: "
                << result.remainingTokens << "\n";
        }
        else {
            std::cout << "Request " << i << " rejected. Retry after: "
                      << result.retryAfterSeconds << " seconds\n";
        }
    }


    return 0;
}