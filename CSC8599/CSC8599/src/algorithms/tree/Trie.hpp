#ifndef TRIE_HPP
#define TRIE_HPP

#include <string>
#include <vector>
#include <stdexcept>

/**
 * @brief trie namespace to hold together all classes related to trie
*/
namespace trie 
{
    /**
     * @brief range structure to represent bounded range
    */
    struct Range 
    {
        int lower;
        int upper;

        int CalculateRange() 
        {
            return upper - lower + 1;
        }

        bool Contains(int i) 
        {
            return i >= lower && i <= upper;
        }
    };

    // typedef for list of ranges
    typedef std::vector<Range> charset;

    // charsets for keys
    const charset ascii_letters     = { { 'A', 'Z' }, { 'a', 'z' } };
    const charset ascii_lowercase   = { { 'a', 'z' } };
    const charset ascii_uppercase   = { { 'A', 'Z' } };
    const charset digits            = { { '0', '9' } };
    const charset alpha_numeric     = { { '0', '9' }, { 'A', 'Z' }, { 'a', 'z' } };

    //============================================================================================//
    //trie node structure
    //============================================================================================//

    template <typename T>
    struct node 
    {
        //============================================================================================//
        //Trie values
        //============================================================================================//

        bool                exists;     // if data exists
        T                   data;       // data at node
        struct node<T>**    children;   // array of children

        //============================================================================================//
        //Accessor
        //============================================================================================//

        // traverse into this node and its children
        // send data to callback if data exists
        void Traverse(void(*itemViewer)(T data), unsigned int noChildren) 
        {
            // if has data, call callback
            if (exists) 
            {
                itemViewer(data);
            }

            // iterate through children
            if (children) 
            {
                for (int i = 0; i < noChildren; i++) 
                {
                    if (children[i]) 
                    {
                        children[i]->Traverse(itemViewer, noChildren);
                    }
                }
            }
        }
    };

    //============================================================================================//
    //Trie class
    //============================================================================================//

    template <typename T>
    class Trie 
    {
    public:

        // default and give specific charset
        Trie(charset chars = alpha_numeric)
            : chars(chars), noChars(0), root(nullptr) 
        {
            // set number of chars
            for (Range r : chars) 
            {
                noChars += r.CalculateRange();
            }

            // initialize root memory
            root = new node<T>;
            root->exists = false;
            root->children = new node<T> *[noChars];
            for (int i = 0; i < noChars; i++) 
            {
                root->children[i] = NULL;
            }
        }

        // insertion (can also use to change data)
        bool Insert(std::string key, T element) 
        {
            int idx;
            node<T>* current = root;

            // iterate through sequential key
            for (char c : key) 
            {
                // convert to index
                idx = GetIdx(c);
                if (idx == -1) 
                {
                    // not found
                    return false;
                }

                // if child doesn't exist, create
                if (!current->children[idx]) 
                {
                    current->children[idx] = new node<T>;
                    current->children[idx]->exists = false;
                    current->children[idx]->children = new node<T> *[noChars];
                    for (int i = 0; i < noChars; i++) 
                    {
                        current->children[idx]->children[i] = NULL;
                    }
                }

                // go to child
                current = current->children[idx];
            }

            // set data
            current->data = element;
            current->exists = true;

            return true;
        }

        // deletion method
        bool Erase(std::string key) 
        {
            if (!root) 
            {
                // no data exists
                return false;
            }

            // pass lambda function that sets target element->exists to false
            return FindKey<bool>(key, [](node<T>* element) -> bool {
                if (!element) 
                {
                    // element is nullptr
                    return false;
                }

                // tell compiler this node has no data
                element->exists = false;
                return true;
                });
        }

        // release root note
        void Cleanup() 
        {
            UnloadNode(root);
        }

        /*
            accessors
        */

        // determine if key is contained in trie
        bool ContainsKey(std::string key) 
        {
            // pass lambda function that obtains if found element exists
            return FindKey<bool>(key, [](node<T>* element) -> bool 
                {
                if (!element) 
                {
                    // element is nullptr
                    return false;
                }

                return element->exists;
                });
        }

        // obtain data element
        T& operator[](std::string key) 
        {
            // pass lambda function that returns data if element exists
            return FindKey<T&>(key, [](node<T>* element) -> T& 
                {
                if (!element || !element->exists) 
                {
                    // element is nullptr
                    throw std::invalid_argument("key not found");
                }

                return element->data;
                });
        }

        // traverse through all keys
        void Traverse(void(*itemViewer)(T data)) 
        {
            if (root) 
            {
                root->Traverse(itemViewer, noChars);
            }
        }

    private:
        charset         chars;      // character set
        unsigned int    noChars;    // length of set
        node<T>*        root;       // root node

        // find element at key and process it
        template <typename V>
        V FindKey(std::string key, V(*process)(node<T>* element)) 
        {
            // placeholders
            int idx;
            node<T>* current = root;
            for (char c : key) 
            {
                idx = GetIdx(c);

                if (idx == -1) 
                {
                    // leave to parameter function to deal with nullptr
                    return process(nullptr);
                }

                // update current
                current = current->children[idx];
                if (!current) 
                {
                    // leave to parameter function to deal with nullptr
                    return process(nullptr);
                }
            }
            return process(current);
        }

        // get index at specific character in character set
        // return -1 if not found
        int GetIdx(char c) 
        {
            int ret = 0;

            for (Range r : chars) 
            {
                if (r.Contains((int)c)) 
                {
                    // found character in range
                    ret += (int)c - r.lower;
                    break;
                }
                else 
                {
                    ret += r.CalculateRange();
                }
            }

            // went through all ranges and found nothing, return -1
            return ret == noChars ? -1 : ret;
        }

        // unload node and its children
        void UnloadNode(node<T>* top) 
        {
            if (!top) {
                return;
            }

            for (int i = 0; i < noChars; i++) 
            {
                if (top->children[i]) 
                {
                    // child exists, deallocate it
                    UnloadNode(top->children[i]);
                }
            }

            // set node to nullptr
            top = nullptr;
        }
    };
}

#endif