//
// Created by 17961 on 2019/6/10.
//

#ifndef BTREE_HPP
#define BTREE_HPP

#include "utility.hpp"
#include <functional>
#include <cstddef>

#include <map>
#include <fstream>
#include <cstdio>
#include <cstring>
using namespace std;
namespace sjtu {

	template<class Key, class Value, class Compare = std::less<Key> >
	class BTree {
		static const int M = 228;
		static const int L = 32;
		class const_iterator;

		class iterator {
			friend class BTree;


			// Your private members go here
			int offset;
			BTree *currentTree;


			bool modify(const Value &value) {
				return true;
			}

			iterator() {
				// TODO Default Constructor
				currentTree = nullptr;
				offset = 0;
			}

			iterator(BTree *b, int p = 0) {
				currentTree = nullptr;
				offset = p;
			}

			iterator(const iterator &other) {
				// TODO Copy Constructor
				currentTree = other.currentTree;
				offset = other.offset;
			}

			// Return a new iterator which points to the n-next elements
			iterator operator++(int) {
				// Todo iterator++
			}

			iterator &operator++() {
				// Todo ++iterator
			}

			iterator operator--(int) {
				// Todo iterator--
			}

			iterator &operator--() {
				// Todo --iterator
			}

			// Overloaded of operator '==' and '!='
			// Check whether the iterators are same
			bool operator==(const iterator &rhs) const {
				// Todo operator ==
			}

			bool operator==(const const_iterator &rhs) const {
				// Todo operator ==
			}

			bool operator!=(const iterator &rhs) const {
				// Todo operator !=
			}

			bool operator!=(const const_iterator &rhs) const {
				// Todo operator !=
			}
		};

		class const_iterator {
			// it should has similar member method as iterator.
			//  and it should be able to construct from an iterator.

			// Your private members go here

			const_iterator() {
				// TODO
			}

			const_iterator(const const_iterator &other) {
				// TODO
			}

			const_iterator(const iterator &other) {
				// TODO
			}
			// And other methods in iterator, please fill by yourself.
		};
		void fileread(void *place, int offset, int num, int size) {
			fseek(file, offset, 0);
			fread(place, num, size, file);
			fflush(file);
		}

		void filewrite(void *place, int offset, int num, int size) {
			fseek(file, offset, 0);
			fwrite(place, num, size, file);
			fflush(file);
		}

	private:
		// Your private members go here
		FILE *file;
		char name[50];



	public:


		struct basic_information {
			int head;
			int tail;
			size_t tree_size;
			int root;
			int end;

			basic_information() {
				head = tail = tree_size = root = end = 0;
			}
		};

		basic_information basic_info;

		struct leaf_node {
			int parent;
			int prev;
			int next;
			int num;
			int offset;

			pair<Key, Value> data[L + 1];

			leaf_node(int t = 0) {
				parent = prev = next = num = 0;
				offset = t;
				memset(data, 0, L + 1);
			}
		};

		struct inter_node {
			int parent;
			int num;
			int offset;
			bool son_type;
			int son[M + 1];
			Key data[M + 1];

			inter_node(int t = 0) {
				parent = num = son_type = 0;
				offset = t;
				memset(son, 0, M + 1);
				memset(data, 0, M + 1);
			}
		};




		BTree() {
			strcpy(name, "123.txt");
			file = fopen(name, "rb+");

			if (file == nullptr) {
				file = fopen(name, "wb+");
				basic_info.tree_size = 0;
				basic_info.end = sizeof(basic_information);

				inter_node root(basic_info.end);
				root.son_type = 1;
				root.num = 1;
				basic_info.root = root.offset;
				basic_info.end += sizeof(inter_node);

				leaf_node leaf(basic_info.end);
				basic_info.head = basic_info.tail = leaf.offset;
				basic_info.end += sizeof(leaf_node);

				root.son[0] = leaf.offset;
				leaf.parent = root.offset;

				filewrite(&basic_info, 0, 1, sizeof(basic_information));
				filewrite(&root, root.offset, 1, sizeof(inter_node));
				filewrite(&leaf, leaf.offset, 1, sizeof(leaf_node));
			}
			else {
				fileread(&basic_info, 0, 1, sizeof(basic_information));
			}
			fflush(file);
		}

		BTree(const BTree &other) {}

		BTree &operator=(const BTree &other) {}

		~BTree() {
			// Todo Destructor
			fclose(file);
		}

		void clear() {
			basic_info.tree_size = 0;
			basic_info.end = sizeof(basic_information);

			inter_node root(basic_info.end);
			root.son_type = 1;
			root.num = 1;
			basic_info.root = root.offset;
			basic_info.end += sizeof(inter_node);

			leaf_node leaf(basic_info.end);
			basic_info.head = basic_info.tail = leaf.offset;
			basic_info.end += sizeof(leaf_node);

			root.son[0] = leaf.offset;
			leaf.parent = root.offset;

			filewrite(&basic_info, 0, 1, sizeof(basic_information));
			filewrite(&root, root.offset, 1, sizeof(inter_node));
			filewrite(&leaf, leaf.offset, 1, sizeof(leaf_node));
		}

		pair<iterator, OperationResult> insert(const Key &key, const Value &value) {
			int leafOffset = find_pos(key, basic_info.root);
			leaf_node leaf;

			if (basic_info.tree_size == 0 || leafOffset == 0) {
				OperationResult t = min_insert(leaf, leafOffset, key, value);
				return pair<iterator, OperationResult>(iterator(), t);
			}

			fileread(&leaf, leafOffset, 1, sizeof(leaf_node));
			OperationResult t = leaf_insert(leaf, key, value);
			fflush(file);
			return pair<iterator, OperationResult>(iterator(), t);
		}


		OperationResult leaf_insert(leaf_node &leaf, const Key &key, const Value &value) {
			int pos = 0;

			for (; pos < leaf.num; pos++) {
				if (key == leaf.data[pos].first)
					return Fail;
				if (key < leaf.data[pos].first)
					break;
			}

			for (int i = leaf.num - 1; i >= pos; --i) {
				leaf.data[i + 1].first = leaf.data[i].first;
				leaf.data[i + 1].second = leaf.data[i].second;
			}

			leaf.num++;
			basic_info.tree_size++;


			leaf.data[pos].first = key;
			leaf.data[pos].second = value;

			filewrite(&basic_info, 0, 1, sizeof(basic_information));

			if (leaf.num <= L)
				filewrite(&leaf, leaf.offset, 1, sizeof(leaf_node));
			else
				leaf_split(leaf, key);

			fflush(file);

			return Success;
		}

		void internode_insert(inter_node &node, const Key &key, int newSon) {
			int pos = 0;
			for (; pos < node.num; pos++)
				if (key < node.data[pos]) break;
			for (int i = node.num - 1; i >= pos; i--)
				node.data[i + 1] = node.data[i];
			for (int i = node.num - 1; i >= pos; i--)
				node.son[i + 1] = node.son[i];
			node.data[pos] = key;
			node.son[pos] = newSon;
			node.num++;
			if (node.num <= M) filewrite(&node, node.offset, 1, sizeof(inter_node));
			else internode_split(node);
			fflush(file);
		}

		void leaf_split(leaf_node &leaf, const Key &key) {

			leaf_node newLeaf;
			leaf_node nextLeaf;

			newLeaf.num = leaf.num / 2;
			leaf.num -= newLeaf.num;

			newLeaf.offset = basic_info.end;
			basic_info.end += sizeof(leaf_node);
			newLeaf.parent = leaf.parent;

			for (int i = 0; i < newLeaf.num; i++) {
				newLeaf.data[i].first = leaf.data[i + leaf.num].first;
				newLeaf.data[i].second = leaf.data[i + leaf.num].second;
			}

			newLeaf.next = leaf.next;
			newLeaf.prev = leaf.offset;
			leaf.next = newLeaf.offset;

			if (newLeaf.next != 0) {
				/
					fileread(&nextLeaf, newLeaf.next, 1, sizeof(leaf_node));
				nextLeaf.prev = newLeaf.offset;
				filewrite(&nextLeaf, nextLeaf.offset, 1, sizeof(leaf_node));
			}

			if (basic_info.tail == leaf.offset)
				basic_info.tail = newLeaf.offset;

			filewrite(&leaf, leaf.offset, 1, sizeof(leaf_node));
			filewrite(&newLeaf, newLeaf.offset, 1, sizeof(leaf_node));
			filewrite(&basic_info, 0, 1, sizeof(basic_information));

			inter_node parent;
			fileread(&parent, leaf.parent, 1, sizeof(inter_node));

			internode_insert(parent, newLeaf.data[0].first, newLeaf.offset);

			fflush(file);
		}



		void internode_split(inter_node &node) {

			inter_node newNode;
			newNode.num = node.num / 2;
			node.num -= newNode.num;
			newNode.parent = node.parent;
			newNode.son_type = node.son_type;
			newNode.offset = basic_info.end;
			basic_info.end += sizeof(inter_node);

			for (int i = 0; i < newNode.num; ++i) {
				newNode.son[i] = node.son[node.num + i];
				newNode.data[i] = node.data[node.num + i];
			}

			leaf_node leaf;
			inter_node tmp;
			for (int j = 0; j < newNode.num; ++j) {
				if (newNode.son_type) {
					fileread(&leaf, newNode.son[j], 1, sizeof(leaf_node));
					leaf.parent = newNode.offset;
					filewrite(&leaf, leaf.offset, 1, sizeof(leaf_node));
				}
				else {
					fileread(&tmp, newNode.son[j], 1, sizeof(inter_node));
					tmp.parent = newNode.offset;
					filewrite(&tmp, tmp.offset, 1, sizeof(inter_node));
				}
			}



			if (node.offset == basic_info.root) {
				inter_node newRoot;
				newRoot.parent = newRoot.son_type = 0;
				newRoot.offset = basic_info.end;
				basic_info.end += sizeof(inter_node);

				newRoot.num = 2;
				newRoot.data[0] = node.data[0];
				newRoot.data[1] = newNode.data[0];

				newRoot.son[0] = node.offset;
				newRoot.son[1] = newNode.offset;

				basic_info.root = node.parent = newNode.parent = newRoot.offset;

				filewrite(&node, node.offset, 1, sizeof(inter_node));
				filewrite(&newNode, newNode.offset, 1, sizeof(inter_node));
				filewrite(&newRoot, newRoot.offset, 1, sizeof(inter_node));
				filewrite(&basic_info, 0, 1, sizeof(basic_information));
			}
			else {
				inter_node parent;

				filewrite(&basic_info, 0, 1, sizeof(basic_information));
				filewrite(&node, node.offset, 1, sizeof(inter_node));
				filewrite(&newNode, newNode.offset, 1, sizeof(inter_node));

				fileread(&parent, node.parent, 1, sizeof(inter_node));

				internode_insert(parent, newNode.data[0], newNode.offset);
			}
			fflush(file);
		}

		OperationResult min_insert(leaf_node &leaf, int leafOffset, const Key &key, const Value &value) {
			fileread(&leaf, basic_info.head, 1, sizeof(leaf_node));
			OperationResult t = leaf_insert(leaf, key, value);

			if (t == Fail) return t;

			int offset = leaf.parent;
			inter_node node;

			while (offset != 0) {
				fileread(&node, offset, 1, sizeof(inter_node));

				node.data[0] = key;

				filewrite(&node, offset, 1, sizeof(inter_node));

				offset = node.parent;
			}
			fflush(file);
			return t;
		}

		int find_pos(const Key &key, int offset) {
			inter_node p;
			fileread(&p, offset, 1, sizeof(inter_node));
			if (p.son_type) {
				int pos = 0;
				for (; pos < p.num; pos++)
					if (key < p.data[pos]) break;
				if (pos == 0) return 0;
				return p.son[pos - 1];
			}
			else {
				int pos = 0;
				for (; pos < p.num; pos++)
					if (key < p.data[pos]) break;
				if (pos == 0) return 0;
				return find_pos(key, p.son[pos - 1]);
			}
		}

		Value at(const Key &key) {
			int leaf_offset = find_pos(key, basic_info.root);

			leaf_node leaf;

			fileread(&leaf, leaf_offset, 1, sizeof(leaf_node));
			for (int i = 0; i < leaf.num; i++)
				if (leaf.data[i].first == key)
					return leaf.data[i].second;
		}

		// Erase: Erase the Key-Value
		// Return Success if it is successfully erased
		// Return Fail if the key doesn't exist in the database

		OperationResult erase(const Key &key) {     //insert写到自闭
			// TODO erase function
			return Fail;  // If you can't finish erase part, just remaining here.
		}

		bool empty() const {
			return basic_info.tree_size == 0;
		}

		// Return the number of <K,V> pairs
		size_t size() const {
			return basic_info.tree_size;
		}

	};
} // namespace sjtu

#endif //CLION_WORK_BTREE_HPP
