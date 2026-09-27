/**
 * @file main.cpp
 * @brief Thread-Safe Distributed Raft Consensus Engine (C++17)
 * @author Ayoub Lahmar (@Redayoub-lang)
 * @version 2.0
 */

#include 
#include 
#include 
#include 
#include 

enum class NodeState { FOLLOWER, CANDIDATE, LEADER };

struct RequestVoteArgs {
    int term;
    int candidate_id;
};

struct RequestVoteReply {
    int term;
    bool vote_granted;
};

class RaftNode {
private:
    int node_id;
    int cluster_size;
    std::atomic current_term{0};
    std::atomic voted_for{-1};
    std::atomic state{NodeState::FOLLOWER};
    mutable std::mutex node_mutex;

public:
    RaftNode(int id, int total_nodes) : node_id(id), cluster_size(total_nodes) {}

    RequestVoteReply handleRequestVote(const RequestVoteArgs& args) {
        std::lock_guard lock(node_mutex);
        RequestVoteReply reply{current_term.load(), false};

        if (args.term > current_term) {
            current_term = args.term;
            state = NodeState::FOLLOWER;
            voted_for = -1;
        }

        if (args.term == current_term && (voted_for == -1 || voted_for == args.candidate_id)) {
            voted_for = args.candidate_id;
            reply.vote_granted = true;
            std::cout << "[NODE " << node_id << "] Voted FOR Candidate " << args.candidate_id << " in Term " << args.term << "\n";
        }
        return reply;
    }

    void startElection(const std::vector>& peer_cluster) {
        {
            std::lock_guard lock(node_mutex);
            state = NodeState::CANDIDATE;
            current_term++;
            voted_for = node_id;
        }

        int votes_granted = 1; // Vote for self
        int quorum = (cluster_size / 2) + 1;

        std::cout << "[NODE " << node_id << "] Initiating Election for Term " << current_term << "\n";

        RequestVoteArgs args{current_term.load(), node_id};

        for (const auto& peer : peer_cluster) {
            if (peer->node_id != this->node_id) {
                RequestVoteReply reply = peer->handleRequestVote(args);
                if (reply.vote_granted) {
                    votes_granted++;
                }
            }
        }

        if (votes_granted >= quorum) {
            std::lock_guard lock(node_mutex);
            state = NodeState::LEADER;
            std::cout << "[NODE " << node_id << "] ELECTED LEADER for Term " << current_term << " (Votes: " << votes_granted << "/" << cluster_size << ")\n";
        } else {
            std::cout << "[NODE " << node_id << "] Election Failed (Quorum Not Met)\n";
        }
    }

    NodeState getState() const { return state.load(); }
    int getTerm() const { return current_term.load(); }
    int getId() const { return node_id; }
};

int main() {
    std::cout << "=========================================================\n";
    std::cout << "   DISTRIBUTED RAFT CONSENSUS ENGINE (C++17)             \n";
    std::cout << "   Developer: Ayoub Lahmar (@Redayoub-lang)              \n";
    std::cout << "=========================================================\n\n";

    int cluster_size = 3;
    std::vector> cluster;

    for (int i = 0; i < cluster_size; ++i) {
        cluster.push_back(std::make_shared(i + 1, cluster_size));
    }

    // Node 1 triggers election across cluster
    cluster[0]->startElection(cluster);

    return 0;
}
