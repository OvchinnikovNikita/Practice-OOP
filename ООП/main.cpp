#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include <algorithm>
#include <set>
#include <stack>

using namespace std;

class Graph
{
private:
	static const int MAXROW = 300; // ����������� �� ������������ ���������� ������ � ��������� ����������. � ������ ������������� ��������
	static const int INF = 10000; // ��������� �������������
	int row = 0; // ���������� ������, 0 �� ���������
	int c_ed = 0; // ����������� ������ � ������� ���������, 0 �� ���������.
	int edge = 0; // ���������� �����, ��� ����� �� �����������, �� �������� �� ������ ������ �������, ����� �����-������ ����������
	vector <vector <int>> vec_Adjacencylist;
	vector <vector <int>> vec_AdjacencyMatrix;
	vector <vector <int>> vec_AdjacencyMatrix_for_Hierholzer; // ����� vec_AdjacencyMatrix, ����� ��� ������ ������ ����� ������. �������� AdjacencyMatrix �����, ������� �������� ��� �����, ���� ���� ������� ��� � �������, ������� ��� �� ������
	vector <vector <int>> vec_IncidenceMatrix;
	vector <int> used; // ������ �������������� ������ ��������� DFS ��������������� ������

	// ���������� ������������ � ��������� ������ ��������
	int timer = 0, t[MAXROW] = { 0 }, f[MAXROW] = {0}; /* t - ������ ������� ������ � �������
		����� f[v] ����� �������� �� ������� ������ � ���� ������� t[v], ����� ������ � ������ ������� p,
		���������� ������ ��������� �����, � ����� �� ���� �������� f[to] ��� ������ ������� to,
		���������� ���������������� ����� v � ������ ������*/

	vector <bool> used_v; // ������ ������, � ������� �������� ��� ���
	set <int> set_to_find_hinges; // ���� ��� ����� ������ ��� ����, ���-�� ������ ������ �������� ���� ���

	// ����� ���������� ��� ������ ��������

	// ������� VP ���������� �� ��������� ������ - �������, � ������������� ��� ����. 
	void VP(int s, int f, vector<vector<int>>& p) // ������� �������������� ���� (���������� �� Floyd_Warshall)
	{	// s - ��������� �������, f - ��������.
		// �������������� ���������� ����������, ������� ��������������� ���� � ��������� ����� �� ��������
		// ����� ��������������� � �������� �� ����� 
		int k = p[s][f];
		if (k == 0)
			return;

		k--;
		VP(s, k, p); // ���� ���� ����� s � k (������ - ��������)
		cout << k + 1 << " ";
		VP(k, f, p); // ���� ���� ����� k � f (�������� - �����)
	}
	
	void DFS_to_find_hinges(int s, vector<bool>& used_v, int p = -1) 
	{// p - �������, ������� ��������������� �� ������� �������� �������, ���� �������� ������� ������, p �� ��������� ��������� -1
		used_v[s] = true;
		timer++;
		t[s] = f[s] = timer;
		int c = 0;

		for (int i = 0; i < vec_Adjacencylist[s].size(); ++i)
		{
			int to = vec_Adjacencylist[s][i];

			if (to == p)  
				continue;

			if (used_v[to])
				f[s] = min(f[s], t[to]); // ������� ��� ���������, ������ ������ ����������� ������� ����������� �������, ��� ������� 

			else 
			{
				DFS_to_find_hinges(to, used_v, s);

				f[s] = min(f[s], f[to]);

				if (f[to] >= t[s] && p != -1) // �����, ����� ������� �� �������� ������, ��� ����� ������ �������
				{
					set_to_find_hinges.insert(s);
				}
					

				++c;
			}
		}
		if (p == -1 && c >= 2) // ���� ������ ��������, ������ �� � �����, ������ p == -1, ���� ����� ������ ��� �����, ��� 2, �� ������ - ������. 
			set_to_find_hinges.insert(s);
	}

	int v_has_a_neighbor(int v) // ������ ��������� �� ������� ������. �������� � �������
	{
		int k = -1;
		for (int i = 0; i < vec_AdjacencyMatrix_for_Hierholzer[v].size(); ++i) //vec_AdjacencyMatrix_for_Hierholzer - ������ ����� ������� ���������.
		{
			k = vec_AdjacencyMatrix_for_Hierholzer[v][i];

			if (k != 0)
			{
				vec_AdjacencyMatrix_for_Hierholzer[v][i] = 0; // ������� ����� �� �������
				vec_AdjacencyMatrix_for_Hierholzer[i][v] = 0;

				return i; // ���������� �������
			}
		}
		return -1; // ���� ��� �������
	}


public:
	void Set_Adjacencylist()
	{
		string s;
		row = 0;
		c_ed = 0;

		while (getline(cin, s))
		{
			row++;

			vec_Adjacencylist.resize(row);

			stringstream s2;
			s2 << s;

			int j;
			vector <int> v_j;

			if (s.size() == 0)
			{
				j = row;
				j--;
				vec_Adjacencylist.at(j).push_back(-1);
			}

			while (s2 >> j)
			{
				edge++;
				v_j.push_back(j);
			}
			for (int i = 0; i < v_j.size(); i++)
			{
				j = row;
				j--;
				vec_Adjacencylist.at(j).push_back(v_j.at(i));
			}

			v_j.clear();
		}
		used.resize(row, 0);
	}

	void Set_AdjacencyMatrix(bool set_adj_list = false)
	{
		if (set_adj_list)
		{
			vec_AdjacencyMatrix.resize(row);
			used.resize(row, 0);
			for (int i = 0; i < vec_AdjacencyMatrix.size(); i++)
			{
				for (int j = 0; j < row; j++)
				{
					vec_AdjacencyMatrix.at(i).push_back(0);
				}
			}

			for (int i = 0; i < vec_Adjacencylist.size(); i++)
			{
				for (int j = 0; j < vec_Adjacencylist.at(i).size(); j++)
				{
					int k = vec_Adjacencylist.at(i).at(j);

					int* p = &vec_AdjacencyMatrix.at(i).at(0);

					for (int w = 0; w < k - 1; w++)
						p++;

					*p = 1;
					c_ed++;
				}
			}
		}
		
		else
		{
			string s;
			row = 0;
			c_ed = 0;

			while (getline(cin, s))
			{
				row++;
				vec_AdjacencyMatrix.resize(row);
				vec_AdjacencyMatrix_for_Hierholzer.resize(row);
				used.resize(row, 0);

				stringstream s2;
				s2 << s;

				int j;
				vector <int> v_j;

				while (s2 >> j)
				{
					if (j != 0)
						edge++;
					v_j.push_back(j);
				}

				for (int i = 0; i < v_j.size(); i++)
				{
					j = row;
					j--;
					vec_AdjacencyMatrix.at(j).push_back(v_j.at(i));
					vec_AdjacencyMatrix_for_Hierholzer.at(j).push_back(v_j.at(i));
				}

				v_j.clear();
			}
		}
	}

	void Print_Adjacencylist()
	{
		for (auto it : vec_Adjacencylist)
		{
			for (auto i : it)
	
				cout << i << " ";

			if (!it.empty()) 
				cout << endl;
			else
			{
				cout << "��� �������" << endl;
			}
		}
		cout << endl;
	}

	void Print_AdjacencyMatrix()
	{		
		for (auto it : vec_AdjacencyMatrix)
		{
			for (auto i : it)
				cout << i << " ";

			cout << endl;
		}	
	}

	void Dfs(int v) 
	{
		v--;
		int j = 0;
		if (used.at(v) == 0)
		{
			cout << ++v << " ";
			v--;
			used.at(v) = 1;
			for (int i = 0; i < vec_AdjacencyMatrix[v].size(); i ++)
			{
				if (vec_AdjacencyMatrix[v][i] == 1)
				{
					Dfs(++i);
					i--;
				}
			}
		}
	}
	
	void Li(int v_s = 0, int v_f = 0) 
	{
		vector <int> bfs_used(row, -1);
		queue <int> q;

		int j; int d = 1;

		j = v_s;
		j--;

		bfs_used.at(j) = 0;
		q.push(v_s);
		
		int k_i = 0, s = 0;
		bool f = false;
		int pred = vec_Adjacencylist.at(v_s - 1).size();
		vector <int> uses(row, -1);
		uses[v_s - 1] = 0;
		while (!q.empty()) // ��������������� �����
		{
			
			int co = 0;
			int k = q.front(); k--;
			for (auto i : vec_Adjacencylist.at(k))
			{
				co = 0;
				int j = i;
				j--;
				if (i != -1 && bfs_used[j] == -1)
				{
					q.push(i);
					cout << "������� " << i << endl;
					bfs_used.at(j) = d;
					uses[j] = 0;

					
					for (auto k : vec_Adjacencylist.at(j))
					{
						if (uses[k - 1] == -1)
						{	
							uses[k - 1] = 0;
							co++;
						}
					}
					k_i++;
					s += co;
					
					cout << "s = " << s << endl;
					cout << "k_i = " << k_i << endl;
				}
			}
			
			q.pop();

			cout << "���� ��������" << endl;
			if (k_i >= pred)
			{
				pred =s;
				s = 0;	
				k_i = 0;
				d++;
			}
		}

		for (auto iter : bfs_used)
		{
			cout << iter << " ";
		}
		cout << endl;
		cout << "---------" << endl;
		vector <int> way;
		way.push_back(v_f);
		int v = v_f, c = 0;
		
		
		while (v != v_s && c != row)
		{
			c++;
			j = v;
			j--;

			for (auto i : vec_Adjacencylist.at(j))
			{
				int j_i = i; 
				j_i--;
	//			cout << bfs_used[j_i] << endl;

				if (bfs_used[j_i] == bfs_used[j] - 1)
				{
					cout << "fd " << endl;
					
					way.push_back(i);
					v = i;
					break;
				}
			}
		}
		cout << endl;

		cout << "way.size() = " << way.size() << endl;
		for (int i = way.size() - 1; i >= 0; i--)
		{
			cout << way[i] << " ";
		}
	} // �������� �������� ��

	void Deykstra(int S, int F)
	{
		int s = --S;
		int f = --F;
		int v;
		vector <int> d(row, INF);
		vector <bool> u(row, false);
		vector <int> p(row);
		d[s] = 0;

		for (int i = 0; i < row; i++)
		{
			int v = -1;
			int min = INF;
			// ������� ����������� ������������ �������
			for (int it = 0; it < row; it++)
			{
				if (u[it]) 
					continue;

				if (d[it] < min)
				{
					min = d[it];
					v = it;
				}
			}
			u[v] = true;

			// ����������� ����� �������

			for (int it = 0; it < row; it++)
			{
				int len = vec_AdjacencyMatrix.at(v).at(it);

				if (len == 0) len = INF;

				if (d[it] > d[v] + len)
				{
					d[it] = d[v] + len;
					p[it] = v;
				}
			}
		}
		cout << "���������� ���� �� ����� " << s + 1 << " �� ���� ��������� �����:" << endl;

		for (int i = 0; i < row; i++)
			cout << d[i] << " ";

		vector <int> vost;

		for (int i = f; i != s; i = p[i])
			vost.push_back(i);

		vost.push_back(s);
		reverse(vost.begin(), vost.end()); 

		cout << endl << "�������������� ����: " << endl;

		for (auto i : vost) cout << i + 1 << " ";
	}

	void Ford_Belman(int S, int F = INF)
	{
		int v_s = --S;
		int v_f = F;

		if (v_f != INF)
			v_f--;

		vector <int> vec(row, INF);
		vector <int> p(row);
		vec[v_s] = 0;

		for (int w = 0; w < row; w++)
		{
			for (int i = 0; i < row; i++)
			{
				if (i == v_f)
				{
					cout << "I = " << i << endl;
					break;
				}

				for (int j = 0; j < row; j++)
				{
					int ves = vec_AdjacencyMatrix[i][j];

					if (ves == 0)
						ves = INF;

					if (vec[j] > vec[i] + ves)
					{
						vec[j] = vec[i] + ves; // ����������� 
						p[j] = i;
					}
				}
			}
		}
		

		for (auto i : vec)
		{
			cout << i << " ";
		}
		
		if (v_f != INF)
		{
			vector <int> vost;

			for (int i = v_f; i != v_s; i = p[i])
				vost.push_back(i);

			vost.push_back(v_s);
			reverse(vost.begin(), vost.end());

			for (auto i : vec)
				cout << i << " ";

			cout << endl << "�������������� ����: " << endl;

			for (auto i : vost)
				cout << i + 1 << " ";
		}
		
	}

	void Floyd_Warshall(int S, int F)
	{
		int s = --S; 
		int f = --F;
		vector <vector <int>> d(row); // ��� ������� ������, � ������� ����� ����������� ��� ��������� 
		
		// �������� ��������� ������ �������� ���������
		for (int i = 0; i < vec_AdjacencyMatrix.size(); ++i)
		{
			for (int j = 0; j < vec_AdjacencyMatrix[i].size(); ++j)
			{
				/* �����. ��� ���� � ������� � ���� ����� �������������, ������ ��� ���������� ������ ���������, ���� �� ������� � ���� ���� 
				������ ���� ����� 0. ������� ������� ��������� ������� ����� ����� ����, � ��������� ������� ���� ����� ������� ���������
				����������� �� INF (� ������� ������ INF �������� �� 10 000)*/
				if (vec_AdjacencyMatrix[i][j] == 0)
				{
					i == j ? d[i].push_back(0) : d[i].push_back(INF); 
					continue;
				}
				d[i].push_back(vec_AdjacencyMatrix.at(i).at(j));
			}
		}

		vector <vector<int>> p(row, vector<int>(row, 0)); // ��������� ������ �������, ����� ������ ��� ����, ����� ������������ ����

		// ���� �������� ������, � �������� ������ ��������� ����� ���������� ����� ��������, �� ������� �� ��� ���� �������� ������� d ����������
		for (int i = 0; i < row; ++i)
		{
			for (int j = 0; j < row; ++j)
				p[i].push_back(0);
		}

		// row - ���������� ������
		// ����� �������� ��������� �������� ����� �������, ������� ������ d
		// ������ �������� ��������� �������� ������ ������, � ���� ����������� ���������� k.
		// ��� ������ ����� ��������, ����� �� ��������� ����, �������� � ���� ����� ������� k, 
		// ���� ����� - �������� ��������, ���� ��� - �������
		for (int k = 0; k < row; ++k)
		{
			for (int i = 0; i < row; ++i)
			{
				for (int j = 0; j < row; ++j)
				{
					if (d[i][j] > d[i][k] + d[k][j])
					{
						d[i][j] = d[i][k] + d[k][j];
						p.at(i).at(j) = k + 1; // k + 1 ������� ��� �������� ��������� � �������� ������� �������������� ����, 
						// ����� ���������� � ������ k, �� �����, ��� ��� ����������, ������� �������������� ���� ����� ���� �������.
						// ��� ������� � �������� ���������� k + 1
					}
				}
			}
		}
		// �������������� ����
		cout << s + 1 << " "; // ������� VP �� ������� ��������� � ��������� �������, ������� ��� �� ��.
		VP(s, f, p);
		cout << f + 1 << " ";
		
	}

	void Johnson(int S)
	{
		// ����� ���������� ���� �� ����� �� ���� ��������� � ������� � ������������������ ������ ��� �������������� �����
		int s = --S;

		// �������� ������� ������
		vector<vector <int>> d(row + 1, vector<int>(row + 1, INF));
		// � ������� ������ ������� d ����� ����, � ������� ������� - �������������, ��� ��������������� �������, ���� �� ������� � ����� 
		// ����� = 0, ���������� ��� ���������� ������� d � ����, ���������� ��� ��������, ��� ���� ��� ���������� ������� � ���������� 
		// ���� �� ����� ����� � ����� �� ����� ��������.

		for (int j = 0; j < row + 1; ++j)
		{
			int i = 0;
			d[i][j] = 0;
		}

		for (int i = 1; i < row + 1; ++i)
		{
			for (int j = 1; j < row + 1; ++j)
			{
				if (vec_AdjacencyMatrix[i - 1][j - 1] == 0 && i != j)
				{
					d[i][j] = INF;
					continue;
				}
				d[i][j] = vec_AdjacencyMatrix[i - 1][j - 1];
			}
		}
		// ��������� ������ - �������� �� ����� 0 (��������������� �������), ����� ����� ��� ���������� ���� �� �� �� ��� ������ �����
		// ������ �������� ����� ������������� ��� ����������� ������� d 

		vector<int> vec(row + 1, INF);
		vec[0] = 0;

		for (int it = 0; it < row + 1; it++)
		{
			for (int i = 0; i < row + 1; i++)
			{
				for (int j = 0; j < row + 1; j++)
				{
					int ves = d[i][j];

					if (vec[j] > vec[i] + ves && vec[i] < INF)
					{
						vec[j] = vec[i] + ves;
					}
				}
			}
		}
		// ������ ������������� ���� �������, �������� ���������� �� ����� � � B ���� = -2, ������ ������ -2 + -5 - - 7 = 0
		// ����������� ������� �������� � ��������� ������� 

		for (int i = 1; i < row + 1; ++i)
		{
			for (int j = 1; j < row + 1; j++)
			{
				if (d[i][j] != 0 && d[i][j] != INF)
				{
					d[i][j] += vec[i] - vec[j];
				}
				else
				{
					d[i][j] = INF;
				}
			}
		}
		// ������ �� ������, � ����� ��������� �������� ��������.
		// ������, ��� ��� i = 0 (j = 0...row + 1), � ��� j = 0 (i = 0 ... row + 1) � ��� �������� ������ (���� � INF) ��������������� �������,
		// ������� � ����������� ��� ��������������� ������� d (��� �������� �������������� ��� ����� �� ����������������� � ���������������, 
		// � ����������� ���������� ����� �� ������ ����� � ������)
		vector <bool> u(row + 1, false); // ������ �������������� ������
		vector <int> vec2(row + 1, INF); // ����� ������� � ���� ������� ���������� ��������� �� s �� ���� ��������� ������. 
										 // ���������� ���������� ���������� ����� INF (�������������)
		vec2[s + 1] = 0; // ����� ��������� �������. �������� ������� � ��� ������ �������� 0.
		for (int i = 1; i < row + 1; ++i) // ������ ������ �������� ��������
		{
			int v = -1;
			int min = INF;
			// ������� ����������� ������������ �������
			for (int it = 1; it < row + 1; ++it)
			{
				if (u[it])
					continue;

				if (vec2[it] < min)
				{
					min = vec2[it];
					v = it;
				}
			}
			u[v] = true;

			// �����������
			for (int it = 1; it < row + 1; ++it)
			{
				int len = d.at(v).at(it);

				if (vec2[it] > vec2[v] + len)
				{
					vec2[it] = vec2[v] + len;
				}
			}
		}

		cout << "���������� ���� �� ����� " << s + 1 << " �� ���� ��������� �����:" << endl;

		for (int i = 1; i < row + 1; i++)
			cout << vec2[i] << " ";
		// ������� ��������, ������ ����� � ������������������ ������ �����������, ��� ������� ��������� ����� ������ � input.txt
		// ��������������� ������� �������� ������� ���. 
	} 

	void Find_hinges(int S)
	{
		used_v.resize(row, false);
		timer = 0;
		DFS_to_find_hinges(S, used_v); // ������� �������� �����������, ��������� � ������� ������ (51 ������)
		
		cout << "��������� ������� ����� �������� �������: " << endl;
		for (auto i : set_to_find_hinges)
		{
			cout << i << " ";
		}
		cout << endl;
	}

	// -------------------- ���������, ������� �� ��� �� ��������. 

	void Hierholzer(int S)
	{
		// ��� ������ ������ ���� ������ ������ ��������� 
		int s = S; // ��������� �������, ����� ���� �����
		int v;

		stack <int> st; // ������� ����
		st.push(s);  

		while (!st.empty()) // ���� ���� �� ����
		{
			v = st.top();
			
			// ������� ��������� � ������� ������. ������ 89.
			int u = v_has_a_neighbor(v); // ��������� �� ������� �������. ���� ���� - ������� ���, � ��������� ������ ����� -1
			
			if (u != -1)
			{
				st.push(u);
			}
			else
			{
				st.pop();
				cout << v << " "; // ������� �������
			}
		}
	}
};

int main()
{
	setlocale(LC_ALL, "Rus");
	freopen("input.txt", "r", stdin);

	Graph graph;
	graph.Set_AdjacencyMatrix();  // ��������� ������� 
	
	graph.Hierholzer(0); // �������� ������� ������ �������� �����. � ����� ����� ���� �������� ����� ���  �����������, ������� ������ �������������

	fclose(stdin);
	return 0;
}