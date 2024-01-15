#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <chrono>
#include <map>
#include <climits>
#include <algorithm>
#include <random>
#include <thread>
#include <mutex>
#include <omp.h>
#include <bitset>
using namespace std;

void getGraph(const string &str, vector<vector<int>> &hyperEdge, unordered_map<int, vector<int>> &hyperNode)
{
    string filename = str;
    ifstream fin(filename, ios::in);
    if (!fin)
        throw runtime_error("Could not open file " + str);
    int count = -1;
    while (true)
    {
        string str;
        getline(fin, str);
        if (str == "")
            break;
        istringstream ss(str);
        int tmp;
        vector<int> e;
        while (ss >> tmp)
        {
            if (find(e.begin(), e.end(), tmp) == e.end())
                e.push_back(tmp);
        }
        if (e.size() == 1)
            continue;
        count++;
        hyperEdge.push_back(e);
        for (auto &node : e)
            hyperNode[node].push_back(count);
    }
}

void density(vector<vector<int>> &hyperEdge, vector<bool> &graph)
{
    int count = 0;
    std::unordered_set<int> uniqueElements;
    for (int i = 0; i < graph.size(); i++)
    {
        if (graph[i] && hyperEdge[i].size() >= 3)
        {
            count++;
            for (auto n : hyperEdge[i])
            {
                uniqueElements.insert(n);
            }
        }
    }
    cout << count << endl;
    cout << count / (double)(uniqueElements.size()) << endl;
}

void triple(vector<vector<int>> &hyperEdge, vector<bool> &graph)
{
    std::map<std::vector<int>, int> freq;
    for (int i = 0; i < hyperEdge.size(); i++)
    {

        if (!graph[i])
            continue;
        auto edge = hyperEdge[i];
        if (edge.size() < 3)
        {
            continue; // ignore if less than a triplet
        }
        std::sort(edge.begin(), edge.end());
        for (auto it = edge.begin(); it != edge.end() - 2; ++it)
        {
            for (auto jt = it + 1; jt != edge.end() - 1; ++jt)
            {
                for (auto kt = jt + 1; kt != edge.end(); ++kt)
                {
                    std::vector<int> triplet = {*it, *jt, *kt};
                    std::sort(triplet.begin(), triplet.end());
                    ++freq[triplet];
                }
            }
        }
    }

    int count = 0;
    for (auto &pair : freq)
    {
        if (pair.second > 1)
        {
            count += pair.second;
        }
    }
    cout<<count<<endl;
}

int main()
{
    vector<vector<int>> hyperEdge;
    unordered_map<int, vector<int>> hyperNode;
    getGraph("NDCC", hyperEdge, hyperNode);
    vector<bool> communityEdge(hyperEdge.size(), false);
    // Set the position of the corresponding edge of the community to 1
    density(hyperEdge, communityEdge);
    triple(hyperEdge, communityEdge);
}