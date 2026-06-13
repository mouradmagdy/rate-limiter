//
// Created by Mourad on 6/12/2026.
//
#include<chrono>
#include<mutex>
#include<iostream>
using Clock=std::chrono::steady_clock;
struct RateLimitResult {
    bool allowed;
    double remainingTokens;
    double retryAfterSeconds;
};

class TokenBucket {
    double capacity;
    double refillRatePerSecond;
    double tokens;

    Clock::time_point lastRefill;
    Clock::time_point lastSeen;

    mutable std::mutex mtx; // because it would be used in a const function ( so mutable to be able to modify it in the function)

    void refill() {
        auto now=Clock::now();
        double elapsedSeconds=std::chrono::duration<double>(now-lastRefill).count();
        if (elapsedSeconds>0) {
            tokens=std::min(capacity,tokens+elapsedSeconds*refillRatePerSecond);
            lastRefill=now;
        }
        lastSeen=now;
    }

public:
    TokenBucket(double capacity, double refillRatePerSecond)
    :capacity(capacity),
    refillRatePerSecond(refillRatePerSecond),
    tokens(capacity),
    lastRefill(Clock::now()),
    lastSeen(Clock::now()){}

    RateLimitResult allow(double cost=1.0) {
        // unlocks automatically (even if error or early return)
        std::lock_guard<std::mutex>lock(mtx);
        refill();
        if (tokens>=cost) {
            tokens-=cost;
            return {
            true,
            tokens,
            0
            };
        }
        double missingTokens=cost-tokens;
        double retryAfter=missingTokens/refillRatePerSecond;
        return {
        false,
        tokens,
        retryAfter
        };
    }

    bool isInactiveFor(std::chrono::seconds duration)const {
        std::lock_guard<std::mutex>lock(mtx);
        auto now=Clock::now();
        return now-lastSeen>=duration;
    }
};