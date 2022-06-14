#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define PID_LENGTH 5
#define MAX_PRODUCT_NAME_LENGTH 25
#define LINE_LENGTH 1025

typedef struct{
    char * PID, * name;
    int pricePerItem;
} Product;

int readInt() {
    char * end;
    char rawInput[LINE_LENGTH];
    scanf("%s", rawInput);
    return strtol(rawInput, &end, 10);
}

Product parseCSV(char * line) {
    Product product;
    char * end;
    char * token = strtok(line, ", ");
    product.PID = token;
    token = strtok(NULL, ", ");
    product.name = token;
    token = strtok(NULL, ", ");
    product.pricePerItem = strtol(token, &end, 10);
    return product;
}

int validatePID(char * PID) {
    FILE * products;
    char line[LINE_LENGTH];
    products = fopen("Products.csv", "r");
    Product product;
    while (fgets(line, LINE_LENGTH, products)) {
        product = parseCSV(line);
        if (!strcmp(PID, product.PID)) {
            fclose(products);
            return 1;
        }
    }
    fclose(products);
    return 0;
}

Product getDetails(char * PID) {
    Product product;
    product.pricePerItem = 0;
    product.name = "NOT FOUND";
    if (validatePID(PID)) {
        char line[LINE_LENGTH];
        FILE *products;
        products = fopen("Products.csv", "r");
        while (fgets(line, LINE_LENGTH, products)) {
            product = parseCSV(line);
            if (!strcmp(PID, product.PID)) {
                return product;
            }
        }
        fclose(products);
    }
    return product;
}

int addItem(FILE * bill) {
    printf("\nEnter PID:");
    char PID[PID_LENGTH];
    scanf("%s", PID);
    if (validatePID(PID)) {
        printf("Quantity:");
        int quantity = readInt();
        Product prodDetails = getDetails(PID);
        int price = prodDetails.pricePerItem * quantity;
        fprintf(bill, "\n%s\t%s\t%d\t\t\t%d\t\t\t\t%d", PID, prodDetails.name, quantity, prodDetails.pricePerItem, price);
        return price;
    }
    printf("%s is not a valid PID", PID);
    return 0;
}

void printInvoice(FILE * bill, int total) {
    fprintf(bill, "\n\nTotal\t%d", total);
    fprintf(bill, "\n==================END OF INVOICE==================");
    fclose(bill);
}

void makeInvoice() {
    bool isPrinted = false;
    FILE * bill;
    int total = 0;
    bill = fopen("bill.txt", "w");
    fprintf(bill, "\t\t\t\tINVOICE\nPID\t\tName\tQuantity\tPricePerItem\tPrice");
    while (!isPrinted) {
        printf("\n[1] Add Item\n[2] Print Bill\n[0] Cancel Bill\nChoose an option:");
        int option = readInt();
        switch (option) {
            case 0:
                return;
            case 1:
                total += addItem(bill);
                break;
            case 2:
                printInvoice(bill, total);
                isPrinted = true;
                break;
            default:
                printf("\nInvalid Option");
                break;
        }
    }
}

int addProduct() {
    FILE * products;
    char PID[PID_LENGTH], name[MAX_PRODUCT_NAME_LENGTH];
    int pricePerItem;
    printf("\nEnter PID:");
    scanf("%s", PID);
    if (validatePID(PID)) return 0;
    products = fopen("Products.csv", "a");
    printf("Enter Product Name:");
    scanf("%s", name);
    printf("Enter Price Per Item:");
    pricePerItem = readInt();
    fprintf(products, "\n%s, %s, %d", PID, name, pricePerItem);
    fclose(products);
    return 1;
}

int deleteProduct() {
    FILE * products, * productsNew;
    char line[LINE_LENGTH], PID[PID_LENGTH];
    products = fopen("Products.csv", "r");
    productsNew = fopen("Products_New.csv", "w");
    Product product;
    printf("\nEnter PID:");
    scanf("%s", PID);
    if (validatePID(PID)) {
        int newLine = 0;
        while (fgets(line, LINE_LENGTH, products)) {
            product = parseCSV(line);
            if (strcmp(PID, product.PID) != 0) {
                if(newLine++) fprintf(productsNew, "\n");
                fprintf(productsNew, "%s, %s, %d", product.PID, product.name, product.pricePerItem);
            }
        }
        fclose(productsNew);
        fclose(products);
        remove("Products.csv");
        rename("Products_New.csv", "Products.csv");
        return 1;
    }
    return 0;
}

void printInventory() {
    FILE * products;
    char line[LINE_LENGTH];
    Product product;
    products = fopen("Products.csv", "r");
    printf("\nPID\tName\tPricePerItem\n");
    while (fgets(line, LINE_LENGTH, products)) {
        product = parseCSV(line);
        printf("%s\t%s\t%d\n", product.PID, product.name, product.pricePerItem);
    }
    fclose(products);
}

int main() {
    printf("\t\tBilling System");
    while (true) {
        printf("\n[1] Make Invoice\n[2] Add New Product\n[3] Delete A Product\n[4] Print Inventory\n[0] Exit\nChoose an option:");
        int option = readInt();
        switch (option) {
            case 0:
                exit(0);
            case 1:
                makeInvoice();
                break;
            case 2:
                if (addProduct()) {
                    printf("Product Added Successfully\n");
                }
                else {
                    printf("Unsuccessful, PID might already exists\n");
                }
                break;
            case 3:
                if (deleteProduct()) {
                    printf("Product Deleted Successfully\n");
                }
                else {
                    printf("Unsuccessful, PID does not exist\n");
                }
                break;
            case 4:
                printInventory();
                break;
            default:
                printf("Invalid Option");
                break;
        }
    }
}
