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
const int N = 2 * 1e6;
omp_lock_t lock;

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

int compute(vector<int> &a, vector<int> &b)
{
    std::unordered_map<int, bool> map;
    for (auto x : a) {
        map[x] = true;
    }

    int count = 0;
    for (auto x : b) {
        if (map.count(x) > 0) {
            count++;
        }
    }
    return count;
}

void getGraph(const string &str, vector<vector<int>> &hyperEdge, vector<vector<int>> &incidentHyperedge, int x)
{
    string filename = "/home/liuyu/C++Projects/graphData/hypergraph/" + str;
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

void output(const string &file, int x, vector<int> &cE)
{
    string filename = "./result/ksCoreValue/" + file + "-" + to_string(x) + "-coreValue.txt";
    ofstream fout;
    fout.open(filename, ios::out);
    if (!fout.is_open())
    {
        cerr << "Cannot open " << filename << " for writing" << endl;
    }
    for (auto &e : cE)
        fout << e << " ";
    fout << endl;
    fout.close();
}

int main()
{
    omp_set_num_threads(100);
    string fileList = "NDCC TaMS NDCS TaAU ThAU ThMS CoMH CoGe";
    std::vector<std::string> files;
    std::istringstream iss(fileList);
    std::string file;
    while (std::getline(iss, file, ' '))
    {
        files.push_back(file);
    }
    vector<vector<double>> time(files.size(), vector<double>(15, 0));
#pragma omp parallel for collapse(2)
    for (int i = 0; i < int(files.size()); i++)
    {
        for (int j = 2; j <= 10; j++)
        {
            vector<vector<int>> hyperEdge;
            vector<vector<int>> incidentHyperedge;
            getGraph(files[i], hyperEdge, incidentHyperedge, j);
            vector<int> cE;
            double t = 0;
            t = ksCoreDecomp(files[i], hyperEdge, incidentHyperedge, j, cE);
            time[i][j] = t;
            output(files[i], j, cE);
        }
    }
    string filename = "./result/time-ksCoreValue.txt";
    ofstream fout(filename, ios::out);
    if (!fout)
    {
        cerr << "Cannot open " << filename << " for writing" << endl;
        exit(1);
    }
    for (int i = 0; i < files.size(); i++)
    {
        fout << files[i] << " ";
        for (int j = 2; j <= 12; j++)
        {
            fout << time[i][j] << " ";
        }
        fout << endl;
    }
    fout.close();
    cout << "All done!" << endl;
}