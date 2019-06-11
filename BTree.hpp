//
// Created by 17961 on 2019/6/10.
//

#ifndef CLION_WORK_BTREE_HPP
#define CLION_WORK_BTREE_HPP

#include "utility.hpp"
#include <functional>
#include <cstddef>
#include <map>
using namespace std;
const int size_M = 500;
const int size_L = 200;
namespace sjtu {
    template <class Key, class Value, class Compare = std::less<Key> >
    class BTree {
    public:
        typedef pair<const Key, Value> value_type;
        typedef size_t position;
        struct fileName{
            char *name;
            fileName(){ name = new char[1]; }
            ~fileName(){if(name) delete []name;}
            void getname(int i) {
                name[0] = static_cast<char>(i);
            }
            void getname(char *n){
                name = n;
            }
        };
        struct Treenode{
            position me, parent;
            position child[size_M + 1];
            Key keylist[size_M + 1];
            int current_size;
            bool rank;
            Treenode(){
                me = parent = current_size = 0;
                rank = 0;
            }
        };
        struct datanode{
            position me, parent;
            position left, right;
            int current_size;
            value_type datalist[size_L + 1];
            datanode(){
                me = parent = 0;
                left = right = 0;
            }
        };
        struct Bas{
            position me, root, head, end, tail;
            int Treesize;
            Bas(){
                root = head = end = tail = Treesize = 0;
            }
        };
        FILE *file, *filefrom;
        Bas bas;
        bool fileisOpen = 0, hf;
        fileName na, filenamefrom;
        position tmpdatapo;
        position tmpnodepo;
        int ustobename = 0;
        position from_tmp = 0;
        class const_iterator;
        class iterator;
        void fileOpen(){
            hf = 1;
            if(fileisOpen == 0){
                file = fopen("2.txt","rb+");
                if(file == NULL){
                    hf = 0;
                    file = fopen("2.txt", "w");
                    fclose(file);
                    file = fopen("2.txt", "rb+");
               }
                else fileread(&bas, 0, sizeof(Bas), 1);
                fileisOpen = 1;
            }
        }
        void fileClose(){
            if(fileisOpen){
                fclose(file);
                fileisOpen = 0;
            }
        }
        void fileread(void *thing, position posi, int size, int num){
            if(fseek(file, posi, 0)) throw"打开失败79";
            fread(thing, size, num, file);
        }
        void filewrite(void *thing, position posi, int size, int num){
            if(fseek(file, posi, 0)) throw "打开失败83";
            fwrite(thing, size, num, file);
        }
        void copyFile(char *to, char *from){
            filenamefrom.getname(from);
            filefrom = fopen(filenamefrom.name, "rb+");
            if(filefrom == NULL) throw "98";
            Bas tmpbas;
            if(fseek(filefrom, 0, 0)) throw"100";
            fread(&tmpbas, sizeof(Bas), 1, filefrom);
            bas.Treesize = tmpbas.Treesize;
            bas.root = bas.end = sizeof(Bas);
            filewrite(&bas, 0, sizeof(Bas), 1);
            copynode(bas.root, tmpbas.root, 0);
            filewrite(&bas, 0, sizeof(Bas), 1);
            fclose(filefrom);
        }
        void copynode(position meposi, position fromposi, position parposi){
            Treenode node, fromnode;
            if(fseek(filefrom, fromposi, 0)) throw"111";
            fread(&fromnode, sizeof(Treenode), 1, filefrom);
            filewrite(&node, meposi, sizeof(Treenode), 1);
            node.parent = parposi;
            node.current_size = fromnode.current_size;
            node.me = meposi;
            node.rank = fromnode.rank;
            for(int i = 0; i < node.current_size; ++i){
                node.keylist[i] = fromnode.keylist[i];
                node.child[i] = fromnode.child[i];
                if(node.rank == 0) copydata(node.child[i], fromnode.child[i], node.me);
                else copynode(node.child[i], fromnode.child[i], node.me);
            }
            filewrite(&node, node.me, sizeof(Treenode), 1);
        }
        void copydata(position meposi, position fromposi, position parposi){
            datanode node, fromnode, leftnode;
            if(fseek(filefrom, fromposi, 0)) throw"130";
            fread(&fromnode, sizeof(datanode), 1, filefrom);
            filewrite(&node, meposi, sizeof(datanode), 1);
            node.me = meposi;
            node.current_size = fromnode.current_size;
            node.parent = fromnode.parent;
            node.left = from_tmp;
            node.right = 0;
            if(from_tmp != 0){
                fileread(&leftnode, from_tmp, 1, sizeof(datanode));
                leftnode.right = meposi;
                filewrite(&leftnode, from_tmp, 1, sizeof(datanode));
                bas.tail = posi;
            }
            else bas.head = posi;
            for(int i = 0;i < node.current_size; ++i) {
                node.datalist[i].first = fromnode.datalist[i].first;
                node.datalist[i].second = fromnode.datalist[i].second;
            }
            filewrite(&node, node.me, sizeof(datanode), 1);
            bas.end += sizeof(datanode);
            from_tmp = posi;
        };
        // Default Constructor and Copy Constructor

        BTree() {
            /*na.getname(ustobename);*/
            file = NULL;
            fileOpen();
            /*Treenode Root;
            datanode Head;
            bas.tail = bas.end = sizeof(Bas) + sizeof(Treenode) + sizeof(datanode);
            bas.head = sizeof(Bas) + sizeof(Treenode);
            bas.Treesize = bas.me = 0;
            Root.parent = 0;
            Root.me = sizeof(Bas);
            Root.rank = 0;
            Head.parent = Root.me;
            Head.parent = sizeof(Treenode) + sizeof(Bas);
            Head.me = sizeof(Bas) + sizeof(Treenode);
            filewrite(&bas, bas.me, sizeof(Bas), 1);
            filewrite(&Root, Root.me, sizeof(Treenode), 1);
            filewrite(&Head, Head.me, sizeof(datanode), 1);
            // Todo Default*/
        }
        BTree(const BTree& other) {
            fileName.setname(ustobename);
            fielOpen();
            copyFile(na.name, other.na.name);
            // Todo Copy
        }
        BTree& operator=(const BTree& other) {
            fileName.setname(ustobename);
            fielOpen();
            copyFile(na.name, other.na.name);
            // Todo Assignment
        }
        ~BTree() {
            fileClose();
            // Todo Destructor
        }
        // Insert: Insert certain Key-Value into the database
        // Return a pair, the first of the pair is the iterator point to the new
        // element, the second of the pair is Success if it is successfully inserted
        position finddatanode(Key &key, position posi){
            Treenode tmp;
            fileread(&tmp, posi, sizeof(Treenode), 1);
            if(!tmp.rank) {
                int i;
                for(i = 0; i < tmp.current_size; ++i) if(key < tmp.keylist[i]) break;
                if(i == 0) return 0;
                else return tmp.child[i - 1];
            }
            else {
                int i;
                for(i = 0; i < tmp.current_size; ++i) if(key < tmp.keylist[i]) break;
                if(i == 0) return 0;
                else return finddatanode(key, child[i - 1]);
            }
        }
        pair<iterator, OperationResult> insert(const Key& key, const Value& value) {

        }
        // Erase: Erase the Key-Value
        // Return Success if it is successfully erased
        // Return Fail if the key doesn't exist in the database
        OperationResult erase(const Key& key) {
            // TODO erase function
            return Fail;  // If you can't finish erase part, just remaining here.
        }
        // Return a iterator to the beginning
        iterator begin() {}
        const_iterator cbegin() const {}
        // Return a iterator to the end(the next element after the last)
        iterator end() {}
        const_iterator cend() const {}
        // Check whether this BTree is empty
        bool empty() const {}
        // Return the number of <K,V> pairs
        size_t size() const {}
        // Clear the BTree
        void clear() {}
        // Return the value refer to the Key(key)
        Value at(const Key& key){
        }
        void split_datanode(datanode &data, const Key &key){
            datanode newdata;
            newdata.current_size = data.current_size - (data.current_size >> 1);
            data.current_size = data.current_size >> 1;
            newdata.me = bas.end;
            bas.end += sizeof(datanode);
            newdata.parent = data.parent;
            for (int i=0; i<newdata.currnet_size; ++i) {
                newdata.datalist[i].first = data.datalist[i + leaf.cnt].first;
                newdata.datalist[i].second = data.datalist[i + leaf.cnt].second;

            }
            newdata.right = data.right;
            newdata.left = data.me;
            data.right = newdata.me;
            datanode rightnode;
            if(newdata.right == 0) info.tail = newdata.me;
            else {
                fileread(&rightnode, newdata.right, sizeof(datanode), 1);
                rightnode.left = newdata.me;
                filewrite(&rightnode, rightnode.me, sizeof(datanode), 1);
            }

            filewrite(&data, data.me, 1, sizeof(datanode));
            filewrite(&newdata, newnode.me, sizeof(datanode), 1);
            filewrite(&bas, 0, sizeof(Bas), 1);

            Treenode par;
            fileread(&par, data.parent, sizeof(Treenode), 1);
            insert_Treenode(par, newdata.datalist[0].first, newdata.me);
        }
        void insert_Treenode(Treenode &node, Key key, position newchild){
            int pos = 0;
            for (; pos < node.cnt; ++pos)
                if (key < node.keylist[pos]) break;
            for (int i = node.cnt - 1; i >= pos; --i)
                node.keylist[i+1] = node.keylist[i];
            for (int i = node.cnt - 1; i >= pos; --i)
                node.ch[i+1] = node.ch[i];
            node.keylist[pos] = key;
            node.child[pos] = newchild;
            ++node.current_size;
            if(node.current_size <= M) filewrite(&node, node.offset, sizeof(internalNode), 1);
            else split_Treenode(node);
        }
        void split_Treenode(Treenode &node){
            Treenode newnode;
            newnode.current_size = node.current_size - (node.current_size >> 1);
            node.current_size = node.current_size >> 1;
            newnode.parent = node.parent;
            newnode.rank = node.rank;
            newnode.me = bas.end;
            bas.end += sizeof(Treenode);
            for (int i = 0; i < newnode.current_size; ++i)
                newnode.keylist[i] = node.key[i + node.current_size];
            for (int i = 0; i < newnode.cnt; ++i)
                newnode.child[i] = node.child[i + node.current_size];


            datanode data;
            Treenode treenode;
            for (int i = 0; i < newnode.current_size; ++i) {
                if(newnode.rank == 0) {
                    fileread(&data, newnode.ch[i], sizeof(leafNode), 1);
                    data.parent = newnode.me;
                    filewrite(&data, data.me, sizeof(leafNode), 1);
                } else {
                    fileread(&treenode, newnode.ch[i], sizeof(internalNode), 1);
                    treenode.parent = newnode.me;
                    filewrite(&treenode, treenode.me, sizeof(internalNode), 1);
                }
            }

            if(node.me == bas.root) {			//分裂根节点

                Treenode newroot;
                newroot.parent = 0;
                newroot.rank = 1;
                newroot.me = info.end;
                bas.end += sizeof(Treenode);
                newroot.current_size = 2;
                newroot.keylist[0] = node.keylist[0];
                newroot.child[0] = node.me;
                newroot.keylist[1] = newnode.keylist[0];
                newroot.child[1] = newnode.me;
                node.parent = newroot.me;
                newnode.parent = newroot.me;
                bas.root = newroot.me;

                filewrite(&bas, 0, sizeof(Bas), 1);
                filewrite(&node, node.me, sizeof(internalNode), 1);
                filewrite(&newnode, newnode.me, sizeof(internalNode), 1);
                filewrite(&newroot, newroot.me, sizeof(internalNode), 1);
            } else {															// not root
                filewrite(&bas, 0, sizeof(Bas), 1);
                filewrite(&node, node.me, sizeof(Treenode), 1);
                filewrite(&newnode, newnode.me, sizeof(Treenode), 1);

                Treenode par;
                readFile(&par, node.parent, sizeof(Treenode), 1);
                insert_node(par, newnode.keylist[0], newnode.me);
            }
        }
        pair <iterator, OperationResult> insert_datanode(datanode data, const Key key, const Value value){
            iterator res;

            int pos;
            for (pos = 0; pos < data.current_size; ++pos) {
                if (key == data.datalist[pos].first) return pair <iterator, OperationResult> (res, Fail);
                if (key < data.datalist[pos].first) break;
            }
            for (int i = data.curretn_size - 1; i >= pos; --i) {
                data.datalist[i+1].first = data.datalist[i].first;
                data.datalist[i+1].second = data.datalist[i].second;
            }
            data.datalist[pos].first = key;
            data.datalist[pos].second = value;
            data.current_size++;
            bas.Treesize++;

            filewrite(&bas, 0, 1, sizeof(Bas));
            if(data.current_size <= L) filewrite(&data, data.me, 1, sizeof(datanode));
            else split_datanode(data, key);
            return pair <iterator, OperationResult> (res, Success);
        }
        pair <iterator, OperationResult>  insert(Key &key, Value &value){
            position dataset = finddatanode(key, bas.root);
            datanode data;
            if(bas.Treesize == 0 || dataset == 0) {
                fileread(&data, bas.head, 1, sizeof(datanode));
                pair <iterator, OperationResult> res = insert_datanode(data, key, value);
                if(res.second == Fail) return ret;
                position parposi = datanode.parent;
                Treenode node;
                while(parposi != 0) {
                    fileread(&node, parposi, 1, sizeof(Treenode));
                    node.keylist[0] = key;
                    filewrite(&node, parposi, 1, sizeof(Treenode));
                    parposi = node.parent;
                }

            }
            else {
                fileread(&data, dataset, 1, sizeof(datanode));
            pair <iterator, OperationResult> res = insert_datanode(datanode, key, value);
            }
            return res;
        }
        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         * The default method of check the equivalence is !(a < b || b > a)
         */
        size_t count(const Key& key) const {}
        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is
         * returned.
         */
        iterator find(const Key& key) {}
        const_iterator find(const Key& key) const {}
        class iterator {
        private:
            // Your private members go here
        public:
            bool modify(const Value& value){

            }
            iterator() {
                // TODO Default Constructor
            }
            iterator(const iterator& other) {
                // TODO Copy Constructor
            }
            // Return a new iterator which points to the n-next elements
            iterator operator++(int) {
                // Todo iterator++
            }
            iterator& operator++() {
                // Todo ++iterator
            }
            iterator operator--(int) {
                // Todo iterator--
            }
            iterator& operator--() {
                // Todo --iterator
            }
            // Overloaded of operator '==' and '!='
            // Check whether the iterators are same
            bool operator==(const iterator& rhs) const {
                // Todo operator ==
            }
            bool operator==(const const_iterator& rhs) const {
                // Todo operator ==
            }
            bool operator!=(const iterator& rhs) const {
                // Todo operator !=
            }
            bool operator!=(const const_iterator& rhs) const {
                // Todo operator !=
            }
        };
        class const_iterator {
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
        private:
            // Your private members go here
        public:
            const_iterator() {
                // TODO
            }
            const_iterator(const const_iterator& other) {
                // TODO
            }
            const_iterator(const iterator& other) {
                // TODO
            }
            // And other methods in iterator, please fill by yourself.
        };
    };
}  // namespace sjtu


#endif //CLION_WORK_BTREE_HPP
