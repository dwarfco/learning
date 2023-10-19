#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define PRINT_BOOK(book) printf("Entry in list:\nTitle:%s\nPrice:%lu\nPages:%lu\nLanguage:%s\nWeight:%.2fkg\nYear Of Publish:%lu\n",\
                book->title, book->price, book->pages, book->language, book->weight/10.0, book->year_of_publishing);

struct book {
    uint8_t title[256];
    uint64_t price;
    uint64_t pages;
    uint8_t language[64];
    uint64_t weight;
    uint64_t year_of_publishing;
} typedef Book;

typedef struct Node Node;
struct Node {
    Book *data;
    Node *prev;
    Node *next;
};

Book *createBook(const uint8_t *title, uint64_t price, uint64_t pages, const uint8_t *language,
                    uint64_t weight, uint64_t year_of_publishing)
{
    Book *newBook = (Book *)calloc(1, sizeof(Book));
    if (!newBook) {return NULL;}
    if (title) {strncpy(newBook->title, title, sizeof(newBook->title));}
    if (language) {strncpy(newBook->language, language, sizeof(newBook->language));}
    newBook->price = price;
    newBook->pages = pages;
    newBook->weight = weight;
    newBook->year_of_publishing = year_of_publishing;
    return newBook;
}

void freeNode(Node *node) {
    Node *NextNode;
    Node *PrevNode;
    if (node) {
        NextNode = node->next;
        PrevNode = node->prev;
        if (PrevNode) {PrevNode->next = node->next;}
        if (NextNode) {NextNode->prev = node->prev;}
        free(node);
    }
}

/* if Where == null, create a new linked list else - insert after Where */
Node *insertBook(Book *newBook, Node *Where) {
    Node *NewNode = NULL;
    if (newBook) {
        NewNode = (Node*)calloc(1, sizeof(Node));
        if (NewNode) {
            /* Populate New Node */
            NewNode->data = newBook;
            NewNode->next = Where ? Where->next : NULL;
            NewNode->prev = Where;
            /* Populate prev node */
            if (Where) {
                if (Where->next) {(Where->next)->prev = NewNode;}
                Where->next = NewNode;
            }
        }
    }
    return NewNode;
}

void insertLast(Book *newBook, Node *head) {
    if (head) {
        while (head->next) {
            head = head->next;
        }
        insertBook(newBook, head);
    }
}

Node *findBookByTitle(Node *head, const uint8_t *title) {
    if (!title) {return NULL;}
    while (head) {
        if (strcmp(head->data->title, title) == 0) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

Book *popBookByTitle(Node *head, const uint8_t *title) {
    Node *found = findBookByTitle(head, title);
    if (!found) {return NULL;}
    Book *book = found->data;
    freeNode(found);
    return book;
}

void destroyList(Node *head) {
    Node *deleted = NULL;
    while (head) {
        deleted = head;
        head = head->next;
        free(deleted->data);
        freeNode(deleted);
    }
}

void printList(Node *head) {
    puts("Listing List entries:");
    while (head) {
        Book *book = head->data;
        PRINT_BOOK(book);
        puts("-----------------------------------------");
        head = head->next;
    }
}

int main() {
    Node * head = NULL;
    Book *book1 = createBook("Harry Potter and the Sorcerer's Stone", 19, 320, "English", 156, 1997);
    head = insertBook(book1, NULL);
    Book *book2 = createBook("Harry Potter and the Chamber of Secrets", 20, 352, "German", 167, 1998);
    insertLast(book2, head);
    Book *book3 = createBook("Harry Potter and the Prisoner of Azkaban", 22, 448, "Ukrainian", 176, 1999);
    insertLast(book3, head);
    Book *book4 = createBook("Harry Potter and the Goblet of Fire", 24, 608, "French", 187, 2000);
    insertLast(book4, head);
    Book *book5 = createBook("Harry Potter and the Order of the Phoenix", 25, 816, "Polish", 198, 2003);
    insertLast(book5, head);
    printList(head);
    puts("");
    /* Init finished */

    /* Popping */
    Book *book51 = popBookByTitle(head, "Harry Potter and the Order of the Phoenix");
    PRINT_BOOK(book51);
    Book *book52 = popBookByTitle(head, "Harry Potter and the Order of the Phoenix");
    if (!book52) {
        puts("Specified Value doesn't exist in list");
    }
    free(book51);
    puts("");

    /* Inserting */
    Book *book6 = createBook("Harry Potter and the Half-Blood Prince", 26, 672, "Italian", 201, 2005);
    insertLast(book6, head);
    printList(head);

    destroyList(head);
}
