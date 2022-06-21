#include <iostream>
#include <memory>
#include <algorithm>
#include <string>
#include <fstream>
#include <vector>

static const int INFINITY_NUM = (1 << 20);
static const int MAX_HEIGHT = 1000;

struct TreeNode
{
    TreeNode() : left(NULL), right(NULL), val(0) {}
    TreeNode(int n) : left(NULL), right(NULL), val(n) {}

    TreeNode* left;
    TreeNode* right;
    int val;
};

class AsciiPrinter
{
    class AsciiNode
    {
    public:
        AsciiNode() :
            left(NULL),
            right(NULL),
            edge_length(0),
            height(0),
            parent_dir(0)
        {
        }

    public:
        AsciiNode* left, * right;
        //length of the edge from this node to its children
        int edge_length;
        int height;

        //-1=I am left, 0=I am root, 1=right   
        int parent_dir;

        std::string label;
    };

public:
    AsciiPrinter() :
        m_print_next(0),
        m_gap(3)
    {
        std::fill(m_lprofile, m_lprofile + MAX_HEIGHT, 0);
        std::fill(m_rprofile, m_rprofile + MAX_HEIGHT, 0);
    }

    //prints ascii tree for given Tree structure
    void print_ascii_tree(TreeNode* t)
    {
        if (t == NULL) {
            return;
        }

        AsciiNode* proot = build_ascii_tree(t);
        compute_edge_lengths(proot);

        for (int i = 0; i < proot->height && i < MAX_HEIGHT; i++) {
            m_lprofile[i] = INFINITY_NUM;
        }
        compute_m_lprofile(proot, 0, 0);

        int xmin = 0;
        for (int i = 0; i < proot->height && i < MAX_HEIGHT; i++) {
            xmin = std::min(xmin, m_lprofile[i]);
        }

        for (int i = 0; i < proot->height; i++)
        {
            m_print_next = 0;
            print_level(proot, -xmin, i);
            printf("\n");
        }

        if (proot->height >= MAX_HEIGHT) {
            printf("(This tree is taller than %d, and may be drawn incorrectly.)\n", MAX_HEIGHT);
        }

        free_ascii_tree(proot);
    }


private:
    AsciiNode* build_ascii_tree_recursive(TreeNode* t)
    {
        if (t == NULL) {
            return NULL;
        }

        AsciiNode* node = new AsciiNode();
        node->left = build_ascii_tree_recursive(t->left);
        node->right = build_ascii_tree_recursive(t->right);

        if (node->left != NULL) {
            node->left->parent_dir = -1;
        }

        if (node->right != NULL) {
            node->right->parent_dir = 1;
        }

        node->label = std::to_string(t->val);
        return node;
    }

    //Copy the tree into the ascii node structre
    AsciiNode* build_ascii_tree(TreeNode* t)
    {
        if (t == NULL) {
            return NULL;
        }

        AsciiNode* node = build_ascii_tree_recursive(t);
        node->parent_dir = 0;

        return node;
    }

    //Free all the nodes of the given tree
    void free_ascii_tree(AsciiNode* node)
    {
        if (node == NULL) {
            return;
        }

        free_ascii_tree(node->left);
        free_ascii_tree(node->right);

        delete node;
    }

    //The following function fills in the m_lprofile array for the given tree.
    //It assumes that the center of the label of the root of this tree
    //is located at a position (x,y).  It assumes that the edge_length
    //fields have been computed for this tree.
    void compute_m_lprofile(AsciiNode* node, int x, int y)
    {
        if (node == NULL) {
            return;
        }

        int isleft = (node->parent_dir == -1);
        m_lprofile[y] = std::min(m_lprofile[y], x - (((int)node->label.length() - isleft) / 2));
        if (node->left != NULL)
        {
            for (int i = 1; i <= node->edge_length && y + i < MAX_HEIGHT; i++) {
                m_lprofile[y + i] = std::min(m_lprofile[y + i], x - i);
            }
        }

        compute_m_lprofile(node->left, x - node->edge_length - 1, y + node->edge_length + 1);
        compute_m_lprofile(node->right, x + node->edge_length + 1, y + node->edge_length + 1);
    }

    void compute_m_rprofile(AsciiNode* node, int x, int y)
    {
        if (node == NULL) {
            return;
        }

        int notleft = (node->parent_dir != -1);
        m_rprofile[y] = std::max(m_rprofile[y], x + (((int)node->label.length() - notleft) / 2));
        if (node->right != NULL)
        {
            for (int i = 1; i <= node->edge_length && y + i < MAX_HEIGHT; i++) {
                m_rprofile[y + i] = std::max(m_rprofile[y + i], x + i);
            }
        }

        compute_m_rprofile(node->left, x - node->edge_length - 1, y + node->edge_length + 1);
        compute_m_rprofile(node->right, x + node->edge_length + 1, y + node->edge_length + 1);
    }

    //This function fills in the edge_length and 
    //height fields of the specified tree
    void compute_edge_lengths(AsciiNode* node)
    {
        if (node == NULL) {
            return;
        }

        compute_edge_lengths(node->left);
        compute_edge_lengths(node->right);

        /* first fill in the edge_length of node */
        if (node->right == NULL && node->left == NULL)
        {
            node->edge_length = 0;
        }
        else
        {
            int hmin = 0;
            if (node->left != NULL)
            {
                for (int i = 0; i < node->left->height && i < MAX_HEIGHT; i++) {
                    m_rprofile[i] = -INFINITY_NUM;
                }
                compute_m_rprofile(node->left, 0, 0);
                hmin = node->left->height;
            }
            else
            {
                hmin = 0;
            }
            if (node->right != NULL)
            {
                for (int i = 0; i < node->right->height && i < MAX_HEIGHT; i++) {
                    m_lprofile[i] = INFINITY_NUM;
                }
                compute_m_lprofile(node->right, 0, 0);
                hmin = std::min(node->right->height, hmin);
            }
            else
            {
                hmin = 0;
            }

            int delta = 4;
            for (int i = 0; i < hmin; i++) {
                delta = std::max(delta, m_gap + 1 + m_rprofile[i] - m_lprofile[i]);
            }

            //If the node has two children of height 1, then we allow the
            //two leaves to be within 1, instead of 2 
            if (((node->left != NULL && node->left->height == 1) ||
                (node->right != NULL && node->right->height == 1)) && delta > 4)
            {
                delta--;
            }

            node->edge_length = ((delta + 1) / 2) - 1;
        }

        //now fill in the height of node
        int h = 1;
        if (node->left != NULL)
        {
            h = std::max(node->left->height + node->edge_length + 1, h);
        }
        if (node->right != NULL)
        {
            h = std::max(node->right->height + node->edge_length + 1, h);
        }
        node->height = h;
    }

    //This function prints the given level of the given tree, assuming
    //that the node has the given x cordinate.
    void print_level(AsciiNode* node, int x, int level)
    {
        if (node == NULL) {
            return;
        }

        int isleft = (node->parent_dir == -1);
        if (level == 0)
        {
            int i = 0;
            for (; i < (x - m_print_next - ((node->label.length() - isleft) / 2)); i++) {
                printf(" ");
            }
            m_print_next += i;
            printf(node->label.c_str());
            m_print_next += node->label.length();
        }
        else if (node->edge_length >= level)
        {
            if (node->left != NULL)
            {
                int i = 0;
                for (; i < (x - m_print_next - (level)); i++) {
                    printf(" ");
                }
                m_print_next += i;
                printf("/");
                m_print_next++;
            }
            if (node->right != NULL)
            {
                int i = 0;
                for (; i < (x - m_print_next + (level)); i++) {
                    printf(" ");
                }
                m_print_next += i;
                printf("\\");
                m_print_next++;
            }
        }
        else
        {
            print_level(node->left,
                x - node->edge_length - 1,
                level - node->edge_length - 1);
            print_level(node->right,
                x + node->edge_length + 1,
                level - node->edge_length - 1);
        }
    }

private:
    //used for printing next node in the same level, this is the x coordinate of the next char printed
    int m_print_next;

    int m_lprofile[MAX_HEIGHT];
    int m_rprofile[MAX_HEIGHT];

    //adjust gap between left and right nodes
    const int m_gap;
};

TreeNode* FreeTree(TreeNode* t)
{
    if (t != NULL)
    {
        FreeTree(t->left);
        FreeTree(t->right);
        delete t;
    }

    return NULL;
}

TreeNode* find_min(TreeNode* t)
{
    if (t == NULL)
    {
        return NULL;
    }
    else if (t->left == NULL)
    {
        return t;
    }
    else
    {
        return find_min(t->left);
    }
}

TreeNode* find_max(TreeNode* t)
{
    if (t == NULL)
    {
        return NULL;
    }
    else if (t->right == NULL)
    {
        return t;
    }
    else
    {
        return find_max(t->right);
    }
}

TreeNode* find(int elem, TreeNode* t)
{
    if (t == NULL)
    {
        return NULL;
    }

    if (elem < t->val)
    {
        return find(elem, t->left);
    }
    else if (elem > t->val)
    {
        return find(elem, t->right);
    }
    else
    {
        return t;
    }
}

//Insert i into the tree t, duplicate will be discarded
//Return a pointer to the resulting tree.                 
TreeNode* insert(int value, TreeNode* t)
{
    if (t == NULL)
    {
        TreeNode* new_node = new TreeNode(value);
        return new_node;
    }

    if (value < t->val)
    {
        t->left = insert(value, t->left);
    }
    else if (value > t->val)
    {
        t->right = insert(value, t->right);
    }
    else //duplicate, ignore it
    {
        return t;
    }

    return t;
}

//Deletes node from the tree
// Return a pointer to the resulting tree
TreeNode* remove(int value, TreeNode* t)
{
    if (t == NULL) {
        return NULL;
    }

    TreeNode* tmp_cell = NULL;
    if (value < t->val)
    {
        t->left = remove(value, t->left);
    }
    else if (value > t->val)
    {
        t->right = remove(value, t->right);
    }
    else if (t->left && t->right)
    {
        tmp_cell = find_min(t->right);
        t->val = tmp_cell->val;
        t->right = remove(t->val, t->right);
    }
    else
    {
        tmp_cell = t;
        if (t->left == NULL)
            t = t->right;
        else if (t->right == NULL)
            t = t->left;

        delete tmp_cell;
    }

    return t;
}

std::ostream& operator<< (std::ostream& out, TreeNode* tree){
    AsciiPrinter ascii;
    ascii.print_ascii_tree(tree);
    return out;
}

std::istream& operator>> (std::istream& in, TreeNode* t){
    int value;
    printf("\nEnter num: ");
    in >> value;

    if (value < t->val)
    {
        t->left = insert(value, t->left);
    }
    else if (value > t->val)
    {
        t->right = insert(value, t->right);
    }
    else {
        return in;
    }

    return in;
}
/*
    _30_
   /    \
  10    20
 /     /  \
50    45  35
>>
30 10 50 # # # 20 45 # # 35 # #
*/

void writeToFile(TreeNode* t, std::ofstream& out) {
    if (!t) {
        out << "#" << " ";
    }
    else {
        out << t->val << " ";
        writeToFile(t->left, out);
        writeToFile(t->right, out);
    }
}

TreeNode* readFromFile(std::string path, TreeNode* t) {
    std::ifstream in;
    int temp{};
    in.open(path);
    TreeNode* new_obj = NULL;

    while (in >> temp) {
        new_obj = insert(temp, new_obj);
    }
    in.close();
    return new_obj;
}