#include "DrawTree.h"

using namespace std;

int main(){
    TreeNode* root = NULL;
    std::string path;
    AsciiPrinter ascii;
    ofstream out;
    int num = 0;
    while (1) {
        printf("\nEnter you num of you choice:");
        printf("\n1.Inserting value");
        printf("\n2.Delete value");
        printf("\n3.Write to file");
        printf("\n4.Read from file");
        printf("\n5.Dislay tree");
        printf("\n69.Exit");
        printf("\nYou enter: ");
        int opt = 0;
        cin >> opt;

        switch (opt) {
        case 1:
            printf("\nEnter value that you want add to tree: ");
            cin >> num;
            printf("\nAfter inserting val %d..\n", num);
            root = insert(num, root);
            ascii.print_ascii_tree(root);
            break;
        case 2:
            printf("\nEnter value that you want delete from tree: ");
            cin >> num;
            printf("\nAfter deleting val %d..\n", num);
            root = remove(num, root);
            ascii.print_ascii_tree(root);
            break;
        case 3:
            printf("\nEnter path of file for writing data in file: ");
            cin >> path;
            out.open(path);
            printf("\n");
            if (out.is_open())
                writeToFile(root, out);
            break;
        case 4:
            printf("\nEnter path of file for reading data from file: ");
            cin >> path;
            printf("\n");
            root = readFromFile(path, root);
            ascii.print_ascii_tree(root);
            break;
        case 5:
            printf("\nDisplaying tree:\n");
            ascii.print_ascii_tree(root);
        case 69:
            printf("\nYou exit from program. Can you guess why 69?");
            return 0;
            break;
        default:
            printf("\nYou enter wrong number, try again");
            break;
        }
    }
    return 0;
}
/*
 printf("\nAfter inserting val 10..\n");
    root = insert(10, root);
    ascii.print_ascii_tree(root);

    printf("\nAfter inserting val 5..\n");
    root = insert(5, root);
    ascii.print_ascii_tree(root);

    printf("\nAfter inserting val 15..\n");
    root = insert(15, root);
    ascii.print_ascii_tree(root);

    printf("\nAfter inserting vals 9, 13..\n");
    root = insert(9, root);
    root = insert(13, root);
    ascii.print_ascii_tree(root);

    printf("\nAfter inserting vals 2, 6, 12, 14, ..\n");
    root = insert(2, root);
    root = insert(6, root);
    root = insert(12, root);
    root = insert(14, root);
    ascii.print_ascii_tree(root);

    printf("\n\nAfter deleting a node (14) with no child..\n");
    root = remove(14, root);
    ascii.print_ascii_tree(root);

    printf("\n\nAfter deleting a node (13) with left child..\n");
    root = remove(13, root);
    ascii.print_ascii_tree(root);

    printf("\n\nAfter deleting a node (5) with left and right children..\n");
    root = remove(5, root);
    ascii.print_ascii_tree(root);

    printf("\n\nAfter deleting a node (10, root node) with left and right children..\n");
    root = remove(10, root);
    ascii.print_ascii_tree(root);

    FreeTree(root);
*/