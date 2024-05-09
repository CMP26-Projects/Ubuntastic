
#include "memory.h"
//[Author: Mariam]
///================ INITIOALIZES NODE IN MEMORY ===============///
memoryNode *initializeMemoryNode(int sizet, pair_t *pair, memoryNode *parent)
{
    memoryNode *node = (memoryNode *)malloc(sizeof(memoryNode));
    node->size = sizet;
    node->interval = pair;
    node->process = NULL;
    node->parent = parent;
    node->right = NULL;
    node->left = NULL;
    printf("a new node has created from %d  to %d \n ", pair->start, pair->end);
    return node;
}
///=========FREE THE MEMORY NODE=====================//
void deleteMemoryNode(memoryNode *node)
{
    deletePair(node->interval);
    if (node != NULL)
        free(node);
}
///=========== WHEN THE SIZE OF MEMORY IS GREATER THAN THE NEEDED SIZE WE DIVIDE BY 2==================//
void divideMemory(memoryNode *root)
{
    pair_t *leftPair = initializePair(root->interval->start, root->size / 2 + root->interval->start -1);
    root->left = initializeMemoryNode(root->size / 2, leftPair, root);
    pair_t *rightPair = initializePair(root->interval->start + root->size / 2, root->interval->end);
    root->right = initializeMemoryNode(root->size / 2, rightPair, root);
    printf("memory has been divided into  two  : )  \n");
}

/////============== AFTER DEALLOCATION WE NEED TO MERGE THE TWO PARTS OF MEMORY ==============///
void mergeLeafs(memoryNode *root)
{

    if (root->parent == NULL)
    {
        printf("Cannot merge further. Root node reached.\n");
        return;
    }

    memoryNode *leftNode = root->left;
    memoryNode *rightNode = root->right;
    root->left = NULL;
    root->right = NULL;

    deleteMemoryNode(leftNode);
    deleteMemoryNode(rightNode);

    printf("merged again two parts\n");
}
///// ============INITIALIZE MEMORY ======================//////////////
memory_t *initializeMemory()
{
    // call this function in schedular to initialize the memory
    memory_t *memory = (memory_t *)malloc(sizeof(memory_t));
    memory->size = 1024;
    pair_t *pair = initializePair(0, 1023);
    memory->root = initializeMemoryNode(1024, pair, NULL);
    memory->totalAllocated = 1024;
    printf("Memory has been initialized \n");
    return memory;
}

///=============== Call this function when you need to allocate a new porccess============//
bool allocateProcess(memory_t *memory, process_t *process)
{
#ifdef DEBUG
    printf("going to add ................ \n");
#endif
    bool flag = false;
    printf("the id is %d\n", process->ID);
    addProcess(memory->root, process, &flag);
    printf("the id is %d\n", process->ID);
    if (flag)
    {
        printf("the process is add sucsessfuly from\n");
        memory->totalAllocated -= process->size;
    }
    else
    {
        printf("there is no place to add the process to memory !\n");
    }

    memoryNode *node = search(memory->root, process);
    if (node != NULL)
        process->interval = node->interval;
    printf("the id is %d\n", process->ID);
    return flag;
}
//=================RECURSION FUNCTION TO ADD NEW NODE IN THE MEMORY TREE=====================//
void addProcess(memoryNode *root, process_t *process, bool *flag)
{
    if (*flag == true)
        return;
    if (root->process != NULL)
    {
        printf("yess the process in the root!\n");
        return;
    }
    if (root->size < process->size && root->left == NULL && root->right == NULL)
    {
        if (root->parent != NULL)
            root = root->parent;
        if (root->right->right == NULL && root->left->left == NULL)
            if (root->right->process == NULL && root->left->process == NULL)
            {
                mergeLeafs(root);
                root->process = process;
                *flag = true;
            }
        return;
    }
    if (root->left == NULL || root->right == NULL)
    {
        divideMemory(root);
    }
    addProcess(root->left, process, flag);
    if (*flag)
        return;
    addProcess(root->right, process, flag);
}
///=============DEALLOCATE MEMORY TAKEN BY THE PROCESS ==================///
//               you need to call it when the process gona terminate
void freeMemory(memory_t *memory, process_t *process)
{
    memoryNode *node = search(memory->root, process);
    if (node == NULL)
    {
        printf("Warinning the process not found !\n");
    }
    else
    {
        printf("the porcess found and going to be freed from memory");
    }
    printf("going to free the size of %d  in memory from %d to %d \n ", process->size, node->interval->start, node->interval->end);
    node->process = NULL;
    bool merge = true;
    while (merge && node->parent != NULL)
    {
        printf("loop\n");
        memoryNode *parent = node->parent;
        if (node->parent->left == node && node->parent->right->process == NULL && node->parent->right->right == NULL)
            mergeLeafs(parent);
        else if (node->parent->right == node && node->parent->left->process == NULL && node->parent->left->left == NULL)
            mergeLeafs(parent);
        else
            merge = false;

        node = node->parent;
    }
    memory->totalAllocated += process->size;
    printf("out of the loop\n");
}
///============FUNCTION THAT SEARCH BY THE PORCESS AND RETURNS THE NODE THAT HAS THAT PORCESS======================///
memoryNode *search(memoryNode *root, process_t *process)
{
    if (root == NULL || process == NULL)
    {
        return NULL;
    }

    if (root->process != NULL && root->process == process)
    {
        return root;
    }

    memoryNode *foundNode = search(root->left, process);
    if (foundNode != NULL)
    {
        return foundNode;
    }

    foundNode = search(root->right, process);
    if (foundNode != NULL)
    {
        return foundNode;
    }

    return NULL;
}
