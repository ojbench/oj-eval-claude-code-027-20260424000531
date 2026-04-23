#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>

class VarState {
public:
    void enterScope() {
        scopes_.push_back({});
    }

    void exitScope() {
        if (scopes_.size() <= 1) {
            throw std::runtime_error("SCOPE UNDERFLOW");
        }
        scopes_.pop_back();
    }

    void set(const std::string& name, int value) {
        // Find existing variable in current or outer scopes
        for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
            if (it->count(name)) {
                (*it)[name] = value;
                return;
            }
        }
        // If not found, create in current scope
        scopes_.back()[name] = value;
    }

    int get(const std::string& name) const {
        for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
            auto sit = it->find(name);
            if (sit != it->end()) {
                return sit->second;
            }
        }
        throw std::runtime_error("VARIABLE NOT DEFINED");
    }

    void clear() {
        scopes_.clear();
        scopes_.push_back({});
    }

    VarState() {
        scopes_.push_back({});
    }

private:
    std::vector<std::unordered_map<std::string, int>> scopes_;
};
