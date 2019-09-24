#include <iostream>
#include <chrono>
#include <utility>
#include <string>
#include <fstream>
#include <map>
#include <vector>
using namespace std;
/*!
*  Narzedzie do zliczania czasu
*
*  Sposob uzycia:
*
*  {
*    Benchmark<std::chrono::nanoseconds> b;
*    // kod do zbadania
*    size_t elapsed = b.elapsed();
*  }
*
*  lub
*
*  {
*    Benchmark<std::chrono::milliseconds> b(true);
*    // kod do zbadania
*  } // obiekt wypisze wartosc czasu w podanych jednostkach na stderr
*/
template<typename D = std::chrono::microseconds>
class Benchmark {
public:

	Benchmark(bool printOnExit = false) : m_print(printOnExit) {
		start = std::chrono::high_resolution_clock::now();
	}
	typename D::rep elapsed() const {
		auto end = std::chrono::high_resolution_clock::now();
		auto result = std::chrono::duration_cast<D>(end - start);
		return result.count();
	}
	~Benchmark() {
		auto result = elapsed();
		if (m_print) {
			std::cerr << "Czas: " << result << "\n";
		}
	}
private:
	std::chrono::high_resolution_clock::time_point start;
	bool m_print = true;
};




/*
* Slownik
*
*/
template<typename KeyType, typename ValueType>
class TreeMap {
public:
	using key_type = KeyType;
	using mapped_type = ValueType;
	using value_type = std::pair<const key_type, mapped_type>;

	//    TreeMap() = default;    // konstruktor trywialny
	//    ~TreeMap() = default;   // destruktor trywialny

private:
	struct SplayNode;
	size_t Splaysize;
	SplayNode *root;
	struct SplayNode {
		SplayNode * up, *left, *right;
		key_type key;
		mapped_type value;
		SplayNode() :up(nullptr), left(nullptr), right(nullptr) {}
	};

	void print(SplayNode *v) {
		if (v == nullptr)
			return;

		print(v->left);
		print(v->right);
		cout << v->key << ":" << v->value << endl;
	}
	// Rotacja L
	void rot_L(SplayNode * & root, SplayNode * A) { //ok
		SplayNode * B = A->right, *p = A->up;

		A->right = B->left;
		if (A->right) A->right->up = A;

		B->left = A;
		B->up = p;
		A->up = B;

		if (p) {
			if (p->left == A) p->left = B;
			else p->right = B;
		} else root = B;
	}

	// Rotacja R
	void rot_R(SplayNode * & root, SplayNode * A) { //ok
		SplayNode * B = A->left, *p = A->up;

		A->left = B->right;
		if (A->left) A->left->up = A;

		B->right = A;
		B->up = p;
		A->up = B;

		if (p) {
			if (p->left == A) p->left = B;
			else p->right = B;
		} else root = B;
	}
	////////////SPLAY
	void Splay(const key_type &key) { //ok
		SplayNode *x, *y;
		x = root;
		y = nullptr;

		if (x) {
			do {
				if (x->key == key) break;
				y = x;
				x = key< x->key ? x->left : x->right;
			} while (x);
			if (!x) x = y;
			while (true) {
				if (!x->up) break;
				if (!x->up->up) {
					if (x->up->left == x) rot_R(root, x->up);
					else rot_L(root, x->up);
					break;
				}

				if ((x->up->up->left == x) && (x->up->left == x)) {
					rot_R(root, x->up->up);
					rot_R(root, x->up);
					continue;
				}
				if ((x->up->up->right == x) && (x->up->right == x)) {
					rot_L(root, x->up->up);
					rot_L(root, x->up);
					continue;
				}

				if (x->up->right == x) {
					rot_L(root, x->up);
					rot_R(root, x->up);
				}

				else {
					rot_R(root, x->up);
					rot_L(root, x->up);
				}
			}
		}
	}


	void insertSplay(const key_type &key, const mapped_type &value) {
		Splaysize++;
		if (Splaysize == 1) {
			root = new SplayNode();
			root->key = key;
			root->value = value;
		} else {
			Splay(key);
			insertSplayRoot(key, value);
		}

	}

	void DFSRelease(SplayNode * v) { //ok
		if (v) {
			DFSRelease(v->left);
			DFSRelease(v->right);
			delete v;
		}
	}

	void insertSplayRoot(const key_type& key, const mapped_type &value) {

		SplayNode *newNode = new SplayNode();
		newNode->key = key;
		newNode->value = value;
		if (root->key > key) {
			if (root->left == nullptr) {
				newNode->right = root;
				root->up = newNode;
				root->left = nullptr;
				root = newNode;
			} else {
				newNode->left = root->left;
				newNode->left->up = newNode;
				root->left = newNode;
				newNode->right = root;
				root->left = nullptr;
				root = newNode;
				root->right->up = root;

				newNode->up = nullptr;
			}
		} else if (root->key <= key) {
			if (root->right == nullptr) {
				newNode->left = root;
				root->up = newNode;
				root->right = nullptr;
				root = newNode;
			} else {
				newNode->right = root->right;
				newNode->right->up = newNode;
				root->right = newNode;
				root->right = nullptr;
				newNode->left = root;
				root = newNode;
				root->left->up = root;

				newNode->up = nullptr;
			}
		}
	}
public:
	TreeMap() { //ok
		Splaysize = 0;
		root = nullptr;
	}
	~TreeMap() { //ok
		DFSRelease(root);
	}
	/*!
	* true jezeli slownik jest pusty
	*/
	bool isEmpty() const { //ok
		return root == nullptr ? true : false;
	}

	/*!
	* dodaje wpis do slownika
	*/
	void insert(const key_type& key, const mapped_type &value) { //ok
		insertSplay(key, value);
	}

	/*!
	* dodaje wpis do slownika przez podanie pary klucz-wartosc
	*/
	void insert(const value_type &key_value) { //ok
		key_type key = key_value.first;
		mapped_type value = key_value.second;
		insertSplay(key, value);
	}

	/*!
	* zwraca referencje na wartosc dla podanego klucza
	*
	* jezeli elementu nie ma w slowniku, dodaje go
	*/
	mapped_type& operator[](const key_type& key) {

		mapped_type value;
		Splay(key);
		if (root->key != key)
			insertSplayRoot(key, value);

		return root->value;
	}

	/*!
	* zwraca wartosc dla podanego klucza
	*/
	const mapped_type& value(const key_type& key) {
		Splay(key);
		return root->value;
	}

	/*!
	* zwraca informacje, czy istnieje w slowniku podany klucz
	*/
	bool contains(const key_type& key) {
		Splay(key);
		return root->key == key;
	}

	/*!
	* zwraca liczbe wpisow w slowniku
	*/
	size_t size() const {
		return Splaysize;
	}

	void printSplay()  {
		print(root);
	}
};
int main() {
	TreeMap<int, string> splay;
	map<int, string> stlmap;
	int count = 5000;
	vector <string> words;
	string fileName = "pan-tadeuszz.txt";
	string readWord;
	fstream file;
	file.open(fileName, ios::in | ios::out);
	if (!file.good()) {
		cout << "File Open Error!" << endl;
		return -5;
	}
	for (int i = 0; i < count; ++i) {
		if (!file.good())
			break;
		file >> readWord;
		words.push_back(readWord);
	}
	cout << "Wczytujemy elementy: " << count << endl;
	cout << "Wstawienie do map\t";
	{
		Benchmark<std::chrono::milliseconds> b(true);
		for (int i = 0; i<count; ++i)
			stlmap[i] = words[i];
	}
	cout << "Wstawienie do Splay\t";
	{
		Benchmark<std::chrono::milliseconds> b(true);
		for (int i = 0; i<count; ++i) {
			splay.insert(i, words[i]);
		}
	}
	splay.printSplay();
	cout << "Wyszukanie w Splay:\t";
	{
		Benchmark<std::chrono::milliseconds> b(true);
		for (int i = count - 1; i >= 0; i--)
			splay.value(i);

	}
	cout << "Wyszukanie w map:\t";
	{
		Benchmark<std::chrono::milliseconds> b(true);
		for (int i = count - 1; i >= 0; i--)
			stlmap.at(i);
	}
	system("pause");
}
