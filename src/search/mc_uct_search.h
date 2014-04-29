#ifndef MC_UCT_SEARCH_H
#define MC_UCT_SEARCH_H

// MCUCTSearch is the "standard" UCT variant that uses Monte-Carlo backups and
// Monte-Carlo sampling for outcome selection.

#include "uct_base.h"

#ifndef NDEBUG
#include <gtest/gtest.h>
#endif

class MCUCTNode { //Monte Carlo UCTNode
public:
    MCUCTNode() :
        children(),
        immediateReward(0.0),
        futureReward(-std::numeric_limits<double>::max()),
        numberOfVisits(0),
        rewardLock(false) {}

    ~MCUCTNode() {
        for (unsigned int i = 0; i < children.size(); ++i) {
            if (children[i]) {
                delete children[i];
            }
        }
    }

    friend class MCUCTSearch;

    void reset() {
        children.clear();
        immediateReward = 0.0;
        futureReward = -std::numeric_limits<double>::max();
        numberOfVisits = 0;
        rewardLock = false;
    }

    double getExpectedRewardEstimate() const {
        return (immediateReward + futureReward) / (double) numberOfVisits;
    }

    double getExpectedFutureRewardEstimate() const {
        return futureReward / (double) numberOfVisits;
    }

    bool isSolved() const {
        return false;
    }

    bool const& isARewardLock() const {
        return rewardLock;
    }

    void setRewardLock(bool const& _rewardLock) {
        rewardLock = _rewardLock;
    }

    int const& getNumberOfVisits() const {
        return numberOfVisits;
    }

    // Print
    void print(std::ostream& out, std::string indent = "") const {
        out << indent << getExpectedRewardEstimate() << " (in " <<
        numberOfVisits << " visits)" << std::endl;
    }

    std::vector<MCUCTNode*> children;

private:

#ifndef NDEBUG
    FRIEND_TEST(uctBaseTest, testUCTSelectionWithLOG);
    FRIEND_TEST(uctBaseTest, testUCTSelectionWithSQRT);
    FRIEND_TEST(uctBaseTest, testUCTSelectionWithLIN);
    FRIEND_TEST(uctBaseTest, testUCTSelectionWithESQRT);
    FRIEND_TEST(uctBaseTest, testSelectUnselectedAction);
    FRIEND_TEST(uctBaseTest, testSelectActionOnRoot);
    FRIEND_TEST(uctBaseTest, testSelectActionRoundRobin);
#endif

    double immediateReward;
    double futureReward;

    int numberOfVisits;

    bool rewardLock;
};

class MCUCTSearch : public UCTBase<MCUCTNode> {
public:
    MCUCTSearch() :
        UCTBase<MCUCTNode>("MC-UCT") {}

protected:
    // Initialization
    void initializeDecisionNodeChild(MCUCTNode* node, unsigned int const& index,
            double const& initialQValue);

    // Outcome selection
    MCUCTNode* selectOutcome(MCUCTNode* node, PDState& nextState, int& varIndex);

    // Backup functions
    void backupDecisionNodeLeaf(MCUCTNode* node, double const& immReward,
            double const& futReward) {
        // This is only different from backupDecisionNode if we want to label
        // nodes as solved, which is not possible in MonteCarlo-UCT.
        backupDecisionNode(node, immReward, futReward);
    }
    void backupDecisionNode(MCUCTNode* node, double const& immReward,
            double const& futReward);
    void backupChanceNode(MCUCTNode* node, double const& futReward);

#ifndef NDEBUG
    FRIEND_TEST(mcUctSearchTest, testInitializeDecisionNodeChild);
    FRIEND_TEST(mcUctSearchTest, testBackupDecisionNode);
#endif
};

#endif
