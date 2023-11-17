#include "../config/config.hpp"
#include <iostream>
#include <optional>
#include <queue>
#include <tuple>
#include <utility> // for std::pair

using namespace std;
using namespace config;

struct BTBEntry
{
    int address;
    queue<bool> history;
    optional<int> targetAddress;

    BTBEntry(int addr, bool hist1, bool hist2, optional<int> targetAddr) : address(addr), targetAddress(targetAddr)
    {
        const int MAX_HISTORY_SIZE = 2;

        history.push(hist1);
        history.push(hist2);

        while (history.size() > MAX_HISTORY_SIZE)
        {
            history.pop();
        }
    }

    void updateHistory(bool newValue)
    {
        const int MAX_HISTORY_SIZE = 2;

        history.push(newValue);

        while (history.size() > MAX_HISTORY_SIZE)
        {
            history.pop();
        }
    }

    void printEntry() const
    {
        cout << "Address: " << address << ", History: [";

        queue<bool> tempHistory = history;
        while (!tempHistory.empty())
        {
            cout << tempHistory.front() << " ";
            tempHistory.pop();
        }

        cout << "], Target Address: " << (targetAddress.has_value() ? to_string(targetAddress.value()) : "N/A") << endl;
    }
};

class BranchTargetBuffer
{
  public:
    BranchTargetBuffer() = default;

    void updateBTB(int address, bool outcome, int targetAddress)
    {
        for (auto& entry : BTB)
        {
            if (entry.address == address)
            {
                entry.targetAddress = targetAddress;
                entry.updateHistory(outcome);
                return;
            }
        }

        // If no matching entry is found, add a new one
        if (BTB.size() == BTB_SIZE)
        {
            BTB.pop_front();
        }
        BTB.push_back(BTBEntry(address, false, false, targetAddress));
    }

    void push(int address, const bool history1, const bool history2, optional<int> targetAddress)
    {

        for (const auto& entry : BTB)
        {
            if (entry.address == address)
            {
                return;
            }
        }

        if (BTB.size() > BTB_SIZE)
        {
            BTB.pop_front();
        }

        BTB.push_back(BTBEntry(address, history1, history2, targetAddress));
    }

    void updateBTB(int address, bool outcome)
    {
        for (auto& entry : BTB)
        {
            if (entry.address == address)
            {
                entry.updateHistory(outcome);
                return;
            }
        }
    }

    pair<bool, optional<int>> predictBranchOutcome(config::Opcode opcode, int address) const
    {
        for (const auto& entry : BTB)
        {
            if (entry.address == address)
            {

                if (entry.targetAddress == nullopt)
                {
                    return {false, nullopt};
                }

                bool prediction = false;

                if (opcode == config::Opcode::BP || opcode == config::Opcode::BNZ)
                {
                    prediction = entry.history.front() || entry.history.back();
                }
                else
                {
                    prediction = entry.history.front() && entry.history.back();
                }

                return make_pair(prediction, entry.targetAddress);
            }
        }

        return {false, nullopt};
    }

    void printBTB() const
    {
        cout << "Branch Target Buffer (BTB):" << endl;
        for (const auto& entry : BTB)
        {
            entry.printEntry();
        }
    }

  private:
    const int BTB_SIZE = 4;
    deque<BTBEntry> BTB;
};