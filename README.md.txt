# Distributed Raft Consensus Engine

![C++17](https://img.shields.io/badge/Language-C%2B%2B17-blue)
![Distributed Systems](https://img.shields.io/badge/Domain-Distributed%20Systems-navy)
![Developer](https://img.shields.io/badge/Developer-Ayoub%20Lahmar-brightgreen)

A lightweight thread-safe implementation of the Raft consensus protocol in C++17, designed for fault-tolerant replicated state machines in distributed network clusters.

Implemented by **Ayoub Lahmar** ([@Redayoub-lang](https://github.com/Redayoub-lang)).

## 📐 Consensus Invariant

To achieve cluster consistency over \(N\) nodes, leader state transitions require a strict majority quorum \(Q\):

$$
Q = \left\lfloor \frac{N}{2} \right\rfloor + 1
$$

Safety invariants guarantee that at most one leader is elected per term \(T\).

## 💻 Build & Compile

```bash
g++ -std=c++17 -O3 main.cpp -o RaftConsensus
./RaftConsensus