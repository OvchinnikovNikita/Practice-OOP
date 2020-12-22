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
	static const int MAXROW = 300; // Ограничение на максимальное количество вершин в некоторых алгоритмах. В случае необходимости изменить
	static const int INF = 10000; // Константа бесконечности
	int row = 0; // количество вершин, 0 по умолчанию
	int c_ed = 0; // колчичество единиц в матрице смежности, 0 по умолчанию.
	int edge = 0; // Количество ребер, ещё нигде не использовал, но алгоритм на всякий случай считает, может когда-нибудь пригодится
	vector <vector <int>> vec_Adjacencylist;
	vector <vector <int>> vec_AdjacencyMatrix;
	vector <vector <int>> vec_AdjacencyMatrix_for_Hierholzer; // Копия vec_AdjacencyMatrix, нужна для работы поиска цикла Эйлера. Изменять AdjacencyMatrix жалко, поэтому имзменяю его копию, весь этот процесс идёт в привате, поэтому мне не стыдно
	vector <vector <int>> vec_IncidenceMatrix;
	vector <int> used; // Вектор использованных вершин алгоритма DFS инициализирован нулями

	// Переменные используются в алгоритме поиска шарниров
	int timer = 0, t[MAXROW] = { 0 }, f[MAXROW] = {0}; /* t - массив времени захода в вершину
		Время f[v] равно минимуму из времени захода в саму вершину t[v], времён захода в каждую вершину p,
		являющуюся концом ОБРАТНОГО ребра, а также из всех значений f[to] для каждой вершины to,
		являющейся непосредственным сыном v в дереве поиска*/

	vector <bool> used_v; // вектор вершин, в которых алгоритм уже был
	set <int> set_to_find_hinges; // Этот сет нужен только для того, что-бы каждый шарнир выводить один раз

	// Конец переменных для поиска шарниров

	// Функция VP вызывается из алгоритма Флойда - Воршала, и предназначена для него. 
	void VP(int s, int f, vector<vector<int>>& p) // функция восстановления пути (вызывается из Floyd_Warshall)
	{	// s - начальная вершина, f - конечная.
		// Восстановление происходит рекурсивно, сначала восстанавливаем путь с начальной точки до середины
		// Потом восстанавливаем с середины до конца 
		int k = p[s][f];
		if (k == 0)
			return;

		k--;
		VP(s, k, p); // Ищем путь между s и k (начало - середина)
		cout << k + 1 << " ";
		VP(k, f, p); // Ищем путь между k и f (середина - конец)
	}
	
	void DFS_to_find_hinges(int s, vector<bool>& used_v, int p = -1) 
	{// p - вершина, которая рассматривалось на прошлой итерации ФУНКЦИИ, если итерация функции первая, p по умолчанию принимает -1
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
				f[s] = min(f[s], t[to]); // Формулы для алгоритма, дальше просто проверяются условия определения вершины, как шарнира 

			else 
			{
				DFS_to_find_hinges(to, used_v, s);

				f[s] = min(f[s], f[to]);

				if (f[to] >= t[s] && p != -1) // Важно, чтобы вершина не являлась корнем, для корня другие правила
				{
					set_to_find_hinges.insert(s);
				}
					

				++c;
			}
		}
		if (p == -1 && c >= 2) // Если первая итерация, значит мы в корне, значит p == -1, если детей больше или равно, чем 2, то корень - шарнир. 
			set_to_find_hinges.insert(s);
	}

	int v_has_a_neighbor(int v) // Просто проверяет на наличие соседа. Работает в матрице
	{
		int k = -1;
		for (int i = 0; i < vec_AdjacencyMatrix_for_Hierholzer[v].size(); ++i) //vec_AdjacencyMatrix_for_Hierholzer - просто копия матрицы смежности.
		{
			k = vec_AdjacencyMatrix_for_Hierholzer[v][i];

			if (k != 0)
			{
				vec_AdjacencyMatrix_for_Hierholzer[v][i] = 0; // Удаляем ребро из матрицы
				vec_AdjacencyMatrix_for_Hierholzer[i][v] = 0;

				return i; // Возвращаем вершину
			}
		}
		return -1; // Если нет соседей
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
				cout << "Нет соседей" << endl;
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
		while (!q.empty()) // Распрастранение волны
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
					cout << "Добавил " << i << endl;
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

			cout << "Цикл закончен" << endl;
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
	} // Волновой алгоритм Ли

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
			// находим минимальную непомеченную вершину
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

			// релаксируем ребра вершины

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
		cout << "Кротчайшие пути от точки " << s + 1 << " до всех остальных точек:" << endl;

		for (int i = 0; i < row; i++)
			cout << d[i] << " ";

		vector <int> vost;

		for (int i = f; i != s; i = p[i])
			vost.push_back(i);

		vost.push_back(s);
		reverse(vost.begin(), vost.end()); 

		cout << endl << "Восстанавливаю путь: " << endl;

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
						vec[j] = vec[i] + ves; // релаксируем 
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

			cout << endl << "Восстанавливаю путь: " << endl;

			for (auto i : vost)
				cout << i + 1 << " ";
		}
		
	}

	void Floyd_Warshall(int S, int F)
	{
		int s = --S; 
		int f = --F;
		vector <vector <int>> d(row); // Наш рабочий вектор, в котором будут происходить все изменения 
		
		// Заполняю двумерный вектор матрицей смежности
		for (int i = 0; i < vec_AdjacencyMatrix.size(); ++i)
		{
			for (int j = 0; j < vec_AdjacencyMatrix[i].size(); ++j)
			{
				/* ВАЖНО. Под нулём в матрице я имел ввиду бесконечность, однако для правильной работы алгоритма, путь из вершины в саму себя 
				должен быть равен 0. поэтому главная диагональ матрицы булет равна нулю, а отсутсвие прямого пути между разными вершинами
				обозначится за INF (в привате класса INF прописан за 10 000)*/
				if (vec_AdjacencyMatrix[i][j] == 0)
				{
					i == j ? d[i].push_back(0) : d[i].push_back(INF); 
					continue;
				}
				d[i].push_back(vec_AdjacencyMatrix.at(i).at(j));
			}
		}

		vector <vector<int>> p(row, vector<int>(row, 0)); // Двумерный вектор предков, нужен только для того, чтобы восстановить путь

		// Пока заполнил нулями, в процессе работы алгоритма будет запоминать номер итерации, на которой то или иное значение вектора d изменилось
		for (int i = 0; i < row; ++i)
		{
			for (int j = 0; j < row; ++j)
				p[i].push_back(0);
		}

		// row - количество вершин
		// Здесь алгоритм выполянет основную часть рабооты, изменяя вектор d
		// Каждую итерацию алгоритму доступно больше вершин, в силу возрастания переменной k.
		// Ему просто нужно выяснить, можно ли сократить путь, добавляя в него новую вершину k, 
		// если можно - значение меняется, если нет - остаётся
		for (int k = 0; k < row; ++k)
		{
			for (int i = 0; i < row; ++i)
			{
				for (int j = 0; j < row; ++j)
				{
					if (d[i][j] > d[i][k] + d[k][j])
					{
						d[i][j] = d[i][k] + d[k][j];
						p.at(i).at(j) = k + 1; // k + 1 сделано для удобства понимания и простоты функции восстановления пути, 
						// можно запоминать и просто k, но тогда, как мне показалось, функция восстановления пути будет чуть сложнее.
						// Мне удобнее и приятнее запоминать k + 1
					}
				}
			}
		}
		// Восстанавливаю путь
		cout << s + 1 << " "; // функция VP не выводит начальную и стартовую вершину, сделаем это за неё.
		VP(s, f, p);
		cout << f + 1 << " ";
		
	}

	void Johnson(int S)
	{
		// Найдёт кратчайшие пути из точки до всех остальных в орграфе с незнакопостоянными весами без отрицательного цикла
		int s = --S;

		// Основной рабочий вектор
		vector<vector <int>> d(row + 1, vector<int>(row + 1, INF));
		// В нулевой строке матрицы d будут нули, в нулевом столбце - бесконечность, это вспомогательная вершина, пути из которой в любую 
		// точку = 0, необходима для приведения матрицы d к виду, доступному для Дейкстры, при этом при приведении матрицы её кратчайшие 
		// пути из любой точки в любую не будут изменены.

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
		// Пройдемся Фордом - Белманом от точки 0 (вспомогательная вершина), чтобы найти все кратчайшие пути из неё во все другие точки
		// Данные значения будут использованны для перерасчёта матрицы d 

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
		// Теперь пересчитываем нашу матрицу, например расстояние из точки А в B было = -2, станет равным -2 + -5 - - 7 = 0
		// Аналогичным обрахом меняются и остальные вершины 

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
		// Теперь всё хорошо, и можно запускать алгоритм Дейкстры.
		// Помним, что при i = 0 (j = 0...row + 1), и при j = 0 (i = 0 ... row + 1) у нас хранятся данные (нули и INF) вспомогательной вершины,
		// Которую я использовал для переопределения вектора d (для процесса преобразования его весов из знаконепостоянных в неотрицательные, 
		// с сохранением кратчайших путей из каждой точки в каждую)
		vector <bool> u(row + 1, false); // вектор использованных вершин
		vector <int> vec2(row + 1, INF); // Будем хранить в этом векторе кратчайшие расстония от s до всех остальных вершин. 
										 // Изначально кратчайшие расстояния равны INF (бесконечности)
		vec2[s + 1] = 0; // кроме стартовой вершины. Значение вектора в ней должно равнятся 0.
		for (int i = 1; i < row + 1; ++i) // Дальше просто алгоритм Дейкстры
		{
			int v = -1;
			int min = INF;
			// находим минимальную непомеченную вершину
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

			// Релаксируем
			for (int it = 1; it < row + 1; ++it)
			{
				int len = d.at(v).at(it);

				if (vec2[it] > vec2[v] + len)
				{
					vec2[it] = vec2[v] + len;
				}
			}
		}

		cout << "Кротчайшие пути от точки " << s + 1 << " до всех остальных точек:" << endl;

		for (int i = 1; i < row + 1; i++)
			cout << vec2[i] << " ";
		// Алгортм работает, пример графа с незнакопостоянными весами прилагается, его матрицу смежности нужно внести в input.txt
		// Вспомогательную вершину алгоритм добавит сам. 
	} 

	void Find_hinges(int S)
	{
		used_v.resize(row, false);
		timer = 0;
		DFS_to_find_hinges(S, used_v); // Функция является рекурсивной, находится в привате класса (51 строка)
		
		cout << "Шарнирами данного графа являются вершины: " << endl;
		for (auto i : set_to_find_hinges)
		{
			cout << i << " ";
		}
		cout << endl;
	}

	// -------------------- Алгоритмы, которые вы ещё не смотрели. 

	void Hierholzer(int S)
	{
		// Для работы должен быть считан список смежности 
		int s = S; // Стартовая вершина, может быть любой
		int v;

		stack <int> st; // рабочий стек
		st.push(s);  

		while (!st.empty()) // пока стек не пуст
		{
			v = st.top();
			
			// функция находится в привате класса. строка 89.
			int u = v_has_a_neighbor(v); // проверяем на наличие соседей. Если есть - выводим его, в противном случае вернёт -1
			
			if (u != -1)
			{
				st.push(u);
			}
			else
			{
				st.pop();
				cout << v << " "; // Выводит вершины
			}
		}
	}
};

int main()
{
	setlocale(LC_ALL, "Rus");
	freopen("input.txt", "r", stdin);

	Graph graph;
	graph.Set_AdjacencyMatrix();  // Считывает матрицу 
	
	graph.Hierholzer(0); // Вызывает функцию поиска Эйлерова цикла. В вашем файле этот алгоритм носит имя  Хирхольцера, поэтому назвал соответсвующе

	fclose(stdin);
	return 0;
}