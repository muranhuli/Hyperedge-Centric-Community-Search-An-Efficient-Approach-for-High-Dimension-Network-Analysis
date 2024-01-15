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

typedef pair<int, int> PII;
unordered_set<int> best;

struct myCmp
{
    bool operator()(const PII &a, const PII &b) const
    {
        if (a.first == b.first)
            return false;
        else
        {
            if (a.second != b.second)
                return a.second < b.second;
            else
                return a.first < b.first;
        }
    }
};

struct myCmp1
{
    bool operator()(const PII &a, const PII &b) const
    {
        if (a.first == b.first)
            return false;
        else
        {
            if (a.second != b.second)
                return a.second > b.second;
            else
                return a.first > b.first;
        }
    }
};

struct myCmp2
{
    bool operator()(const pair<int, double> &a, const pair<int, double> &b) const
    {
        if (a.first == b.first)
            return false;
        else
        {
            if (a.second != b.second)
                return a.second > b.second;
            else
                return a.first > b.first;
        }
    }
};

int compute(vector<int> &a, vector<int> &b)
{
    std::unordered_map<int, bool> map;
    for (auto x : a)
    {
        map[x] = true;
    }

    int count = 0;
    for (auto x : b)
    {
        if (map.count(x) > 0)
        {
            count++;
        }
    }
    return count;
}

void getGraph(const string &str, vector<vector<int>> &hyperEdge, vector<vector<int>> &incidentHyperedge, int x)
{
    string filename = str;
    ifstream fin(filename, ios::in);
    if (!fin)
        throw runtime_error("Could not open file " + str);
    int count = -1;
    unordered_map<int, vector<int>> tmpnode;
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
            e.push_back(tmp);
        }
        if (e.size() < x)
            continue;
        count++;
        hyperEdge.push_back(e);
    }
    incidentHyperedge.resize(count + 1);
    for (int i = 0; i < hyperEdge.size(); i++)
    {
        for (int j = i + 1; j < hyperEdge.size(); j++)
        {
            // This is more time consuming
            if (compute(hyperEdge[i], hyperEdge[j]) >= x)
            {
                incidentHyperedge[i].push_back(j);
                incidentHyperedge[j].push_back(i);
            }
        }
    }
}

double ksCoreDecomp(const string &file, vector<vector<int>> &hyperEdge, vector<vector<int>> &incidentHyperedge, int x, vector<int> &cE)
{
    auto t1 = std::chrono::steady_clock::now();
    cE.resize(hyperEdge.size(), 0);
    vector<int> dE(hyperEdge.size(), 0);
    vector<bool> visitedEdge(hyperEdge.size(), false);
    for (int i = 0; i < hyperEdge.size(); i++)
    {
        dE[i] = int(incidentHyperedge.at(i).size());
    }
    set<PII, myCmp> Q;
    for (int i = 0; i < dE.size(); i++)
    {
        Q.insert(make_pair(i, dE[i]));
    }
    int k = 1;
    while (!Q.empty())
    {
        PII p = *Q.begin();
        Q.erase(Q.begin());
        k = max(k, p.second);
        cout << p.first << " " << p.second << endl;
        cE[p.first] = k;
        visitedEdge[p.first] = true;
        for (auto edge : incidentHyperedge[p.first])
        {
            if (visitedEdge[edge])
                continue;
            if (Q.erase(make_pair(edge, dE[edge])))
            {
                dE[edge]--;
                Q.insert(make_pair(edge, dE[edge]));
            }
        }
    }
    auto t2 = std::chrono::steady_clock::now();
    double dr_ns = std::chrono::duration<double, std::nano>(t2 - t1).count();
    return dr_ns;
}

int MinSize(vector<vector<int>> &incidentHyperedge, int K, unordered_set<int> C, unordered_set<int> P)
{
    unordered_map<int, int> r;
    set<PII, myCmp1> S;
    int lb = C.size();
    for (auto e : C)
    {
        r[e] = K - incidentHyperedge[e].size();
        S.insert(make_pair(e, r[e]));
    }
    while (!S.empty())
    {
        PII p = *S.begin();
        S.erase(S.begin());
        if (p.second < 0)
        {
            break;
        }
        lb += p.second;

        set<PII, myCmp1> S1;
        for (auto e : P)
        {
            unordered_set<int> tmp;
            for (auto x : incidentHyperedge[e])
            {
                if (C.count(x))
                {
                    tmp.insert(x);
                }
            }
            S1.insert(make_pair(e, tmp.size()));
        }
        int cnt = 0;
        while (!S1.empty() && cnt < p.second)
        {
            PII p1 = *S1.begin();
            S1.erase(S1.begin());
            for (auto e : incidentHyperedge[p1.first])
            {
                if (C.count(e))
                    continue;
                if (S.erase(make_pair(e, r[e])))
                {
                    r[e]--;
                    S.insert(make_pair(e, r[e]));
                }
            }
            cnt++;
        }
    }
    return lb;
}

void OBBAB(vector<vector<int>> &hyperEdge, vector<vector<int>> &incidentHyperedge, int K, unordered_set<int> C, unordered_set<int> P)
{
    if (MinSize(incidentHyperedge, K, C, P) > best.size())
        return;
    bool flag = true;
    for (auto &e : C)
    {
        if (incidentHyperedge[e].size() < K)
        {
            flag = false;
            break;
        }
    }
    if (flag && C.size() <= best.size())
    {
        best = C;
        return;
    }
    if (P.empty())
    {
        return;
    }
    set<pair<int,double>, myCmp2> S;
    for (auto e : P)
    {
        double score = 0;
        for (auto e1 : incidentHyperedge[e])
        {
            if (C.count(e1))
            {
                score += 1.0 / (double)incidentHyperedge[e1].size();
            }
        }
        S.insert(make_pair(e, score));
    }

    int e = S.begin()->first;
    P.erase(e);
    unordered_set<int> C1 = C;
    C1.insert(e);
    OBBAB(hyperEdge, incidentHyperedge, K, C1, P);
    OBBAB(hyperEdge, incidentHyperedge, K, C, P);
}

int main()
{
    int queryVertex = 5;
    int K = 5;
    int S = 3;
    string fileName = "";

    vector<vector<int>> hyperEdge;
    vector<vector<int>> incidentHyperedge;
    getGraph(fileName, hyperEdge, incidentHyperedge, S);
    vector<int> cE;
    double t = 0;
    t = ksCoreDecomp(fileName, hyperEdge, incidentHyperedge, S, cE);

    unordered_set<int> P;
    unordered_set<int> C;
    for (int i = 0; i < hyperEdge.size(); i++)
    {
        if (cE[i] >= K)
        {
            P.insert(i);
        }
    }
    best = P;
    OBBAB(hyperEdge, incidentHyperedge, K, C, P);
    cout << best.size() << endl;
}