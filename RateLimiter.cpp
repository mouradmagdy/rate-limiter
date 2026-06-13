//
// Created by Mourad on 6/12/2026.
//
#include <unordered_map>
#include<string>
#include<thread>
#include"TokenBucket.cpp"

class RateLimiter {
private:
    double capacity;
    double refillRatePerSecond;

    std::unordered_map<std::string,std::unique_ptr<TokenBucket>>buckets;

    std::mutex mapMutex;

public:
    RateLimiter(double capacity,double refillRatePerSecond):
    capacity(capacity),
    refillRatePerSecond(refillRatePerSecond){}

    RateLimitResult allowRequest(const std::string&key,double cost=1.0) {
        TokenBucket*bucket=nullptr;
        {
            std::lock_guard<std::mutex>lock(mapMutex);
            auto it=buckets.find(key);
            if (it==buckets.end()) {
                auto newBucket=std::make_unique<TokenBucket>(capacity,refillRatePerSecond);
                bucket=newBucket.get();
                /*unique ptr can't be copied (only moved, so here we transfer the ownership
                 * to the map
                 */
                buckets[key]=std::move(newBucket);
            }
            else {
                // here the ptr still points to the object even after moving to the map
                bucket=it->second.get();
            }
        }
        // a shared pointer here would be more safe, because after unlocking, another thread may clear the
        // bucket so this would leave us with a dangling pointer
        return bucket->allow(cost);
    }

    void cleanupInactiveUsers(std::chrono::seconds maxInactiveTime) {
        std::lock_guard<std::mutex>lock(mapMutex);
        for (auto it=buckets.begin();it!=buckets.end();) {
            if (it->second->isInactiveFor(maxInactiveTime)) {
                it=buckets.erase(it); // it returns us an it to the next valid element
            }
            else {
                ++it; // ++it is better here bec it++ can make a temporary copy
            }
        }
    }
    size_t activeBuckets()const {
        return buckets.size();
    }
};
