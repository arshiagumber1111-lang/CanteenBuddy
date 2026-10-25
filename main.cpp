
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <limits>
#include <cstdlib>

using namespace std;

struct Food {
    string name;
    double price;
};

struct CartItem {
    Food food;
    int quantity;
};

const string MENU_FILE = "menu.txt";
const string ORDER_FILE = "orders.txt";

// Function declarations
void loadMenuFromFile(vector<Food>& menu);
void saveMenuToFile(const vector<Food>& menu);
void displayMenu(const vector<Food>& menu);
void addFoodToCart(const vector<Food>& menu, vector<CartItem>& cart);
void viewCart(const vector<CartItem>& cart);
void removeFoodFromCart(vector<CartItem>& cart);
void searchFood(const vector<Food>& menu, vector<CartItem>& cart);
void sortMenu(vector<Food>& menu);
void viewOrderHistory();
void finishOrder(vector<CartItem>& cart);
void adminPanel(vector<Food>& menu);
int getIntInput(const string& message);
double getDoubleInput(const string& message);
string getCurrentDateTime();

// Safely read an integer
int getIntInput(const string& message) {
    int value;

    while (true) {
        cout << message;

        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }

        cout << "Invalid input! Please enter a number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// Safely read a price
double getDoubleInput(const string& message) {
    double value;

    while (true) {
        cout << message;

        if (cin >> value && value >= 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }

        cout << "Invalid price! Enter a valid non-negative price.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// Current date and time
string getCurrentDateTime() {
    time_t now = time(nullptr);
    tm* localTime = localtime(&now);

    if (localTime == nullptr) {
        return "Date unavailable";
    }

    stringstream ss;
    ss << put_time(localTime, "%d-%m-%Y %H:%M:%S");
    return ss.str();
}

// Load menu from file
void loadMenuFromFile(vector<Food>& menu) {
    ifstream file(MENU_FILE);

    if (!file.is_open()) {
        // Create the default menu if the file does not exist
        menu = {
            {"Maggi", 40},
            {"Sandwich", 50},
            {"Burger", 60},
            {"Cold Coffee", 70},
            {"Momos", 80},
            {"Noodles", 80},
            {"Pasta", 110},
            {"Pizza", 120}
        };

        saveMenuToFile(menu);
        return;
    }

    menu.clear();
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string name, priceText;

        if (getline(ss, name, ',') &&
            getline(ss, priceText)) {
            try {
                size_t pos;
                double price = stod(priceText, &pos);

                if (pos == priceText.size() &&
                    !name.empty() && price >= 0) {
                    menu.push_back({name, price});
                }
            }
            catch (...) {
                // Ignore an invalid line in menu.txt
            }
        }
    }

    file.close();

    if (menu.empty()) {
        cout << "Warning: menu.txt contains no valid food items.\n";
    }
}

// Save menu to file
void saveMenuToFile(const vector<Food>& menu) {
    ofstream file(MENU_FILE);

    if (!file.is_open()) {
        cout << "Error: Could not save menu.txt\n";
        return;
    }

    for (const Food& item : menu) {
        file << item.name << "," << item.price << "\n";
    }

    file.close();
}

// Display menu
void displayMenu(const vector<Food>& menu) {
    cout << "\n========== FOOD MENU ==========\n";

    if (menu.empty()) {
        cout << "No food items available.\n";
        return;
    }

    cout << left << setw(8) << "Number"
         << setw(22) << "Food"
         << "Price (Rs.)\n";

    cout << "------------------------------------------\n";

    for (size_t i = 0; i < menu.size(); i++) {
        cout << left << setw(8) << i + 1
             << setw(22) << menu[i].name
             << fixed << setprecision(2)
             << menu[i].price << "\n";
    }
}

// Add food to cart
void addFoodToCart(const vector<Food>& menu,
                   vector<CartItem>& cart) {
    if (menu.empty()) {
        cout << "No food items available.\n";
        return;
    }

    displayMenu(menu);

    int number = getIntInput("Enter food number (0 to cancel): ");

    if (number == 0) {
        return;
    }

    if (number < 1 || number > static_cast<int>(menu.size())) {
        cout << "Invalid food number!\n";
        return;
    }

    int quantity = getIntInput("Enter quantity: ");

    if (quantity <= 0) {
        cout << "Quantity must be greater than zero.\n";
        return;
    }

    int index = number - 1;

    for (CartItem& item : cart) {
        if (item.food.name == menu[index].name) {
            item.quantity += quantity;
            cout << "Cart quantity updated successfully!\n";
            return;
        }
    }

    cart.push_back({menu[index], quantity});
    cout << menu[index].name << " added to cart successfully!\n";
}

// View cart
void viewCart(const vector<CartItem>& cart) {
    cout << "\n============== YOUR CART ==============\n";

    if (cart.empty()) {
        cout << "Your cart is empty.\n";
        return;
    }

    double total = 0;

    cout << left << setw(20) << "Food"
         << setw(10) << "Price"
         << setw(10) << "Quantity"
         << "Subtotal\n";

    cout << "--------------------------------------------------\n";

    for (const CartItem& item : cart) {
        double subtotal = item.food.price * item.quantity;
        total += subtotal;

        cout << left << setw(20) << item.food.name
             << setw(10) << fixed << setprecision(2)
             << item.food.price
             << setw(10) << item.quantity
             << subtotal << "\n";
    }

    cout << "--------------------------------------------------\n";
    cout << "Grand Total: Rs. " << fixed << setprecision(2)
         << total << "\n";
}

// Remove a specific quantity from cart
void removeFoodFromCart(vector<CartItem>& cart) {
    if (cart.empty()) {
        cout << "Your cart is empty.\n";
        return;
    }

    viewCart(cart);

    int number = getIntInput(
        "Enter cart item number to remove (0 to cancel): ");

    if (number == 0) {
        return;
    }

    if (number < 1 || number > static_cast<int>(cart.size())) {
        cout << "Invalid cart item number!\n";
        return;
    }

    int index = number - 1;

    int quantity = getIntInput("Enter quantity to remove: ");

    if (quantity <= 0) {
        cout << "Quantity must be greater than zero.\n";
        return;
    }

    if (quantity >= cart[index].quantity) {
        cout << cart[index].food.name << " removed from cart.\n";
        cart.erase(cart.begin() + index);
    } else {
        cart[index].quantity -= quantity;
        cout << "Quantity reduced successfully.\n";
    }
}

// Search food and add it to cart
void searchFood(const vector<Food>& menu,
                vector<CartItem>& cart) {
    string searchName;

    cout << "Enter food name to search: ";
    getline(cin, searchName);

    bool found = false;

    for (size_t i = 0; i < menu.size(); i++) {
        if (menu[i].name.find(searchName) != string::npos) {
            cout << i + 1 << ". " << menu[i].name
                 << " - Rs. " << fixed << setprecision(2)
                 << menu[i].price << "\n";
            found = true;
        }
    }

    if (!found) {
        cout << "Food not found.\n";
        return;
    }

    int number = getIntInput(
        "Enter the displayed food number to add (0 to cancel): ");

    if (number == 0) {
        return;
    }

    if (number < 1 || number > static_cast<int>(menu.size()) ||
        menu[number - 1].name.find(searchName) == string::npos) {
        cout << "Invalid selection.\n";
        return;
    }

    int quantity = getIntInput("Enter quantity: ");

    if (quantity <= 0) {
        cout << "Quantity must be greater than zero.\n";
        return;
    }

    const Food& selected = menu[number - 1];

    for (CartItem& item : cart) {
        if (item.food.name == selected.name) {
            item.quantity += quantity;
            cout << "Cart quantity updated successfully!\n";
            return;
        }
    }

    cart.push_back({selected, quantity});
    cout << selected.name << " added to cart successfully!\n";
}

// Sort menu from low price to high price
void sortMenu(vector<Food>& menu) {
    sort(menu.begin(), menu.end(),
         [](const Food& a, const Food& b) {
             return a.price < b.price;
         });

    cout << "Menu sorted by price (low to high).\n";
    displayMenu(menu);
}

// View saved order history
void viewOrderHistory() {
    ifstream file(ORDER_FILE);

    cout << "\n========== ORDER HISTORY ==========\n";

    if (!file.is_open()) {
        cout << "No saved order history found yet.\n";
        return;
    }

    string line;
    bool hasData = false;

    while (getline(file, line)) {
        cout << line << "\n";
        hasData = true;
    }

    if (!hasData) {
        cout << "No orders recorded yet.\n";
    }

    file.close();
}

// Finish order, generate bill and save order history
void finishOrder(vector<CartItem>& cart) {
    if (cart.empty()) {
        cout << "Your cart is empty. Add food before checkout.\n";
        return;
    }

    double total = 0;

    for (const CartItem& item : cart) {
        total += item.food.price * item.quantity;
    }

    double discount = (total >= 500) ? total * 0.10 : 0;
    double finalAmount = total - discount;

    cout << "\n========== PAYMENT ==========\n";
    cout << "1. Cash\n";
    cout << "2. UPI\n";
    cout << "3. Card\n";

    int paymentChoice = getIntInput("Choose payment method: ");
    string paymentMethod;

    switch (paymentChoice) {
        case 1:
            paymentMethod = "Cash";
            break;
        case 2:
            paymentMethod = "UPI";
            break;
        case 3:
            paymentMethod = "Card";
            break;
        default:
            cout << "Invalid payment option. Checkout cancelled.\n";
            return;
    }

    // Open the file before clearing the cart
    ofstream file(ORDER_FILE, ios::app);

    if (!file.is_open()) {
        cout << "Could not save the order. Checkout cancelled.\n";
        return;
    }

    // Generate a simple order ID
    srand(static_cast<unsigned int>(time(nullptr)));
    int orderID = rand() % 900000 + 100000;

    string dateTime = getCurrentDateTime();

    cout << "\n========== CANTEEN BUDDY BILL ==========\n";
    cout << "Order ID: " << orderID << "\n";
    cout << "Date & Time: " << dateTime << "\n";
    cout << "----------------------------------------\n";

    for (const CartItem& item : cart) {
        double subtotal = item.food.price * item.quantity;

        cout << item.food.name
             << " | Qty: " << item.quantity
             << " | Subtotal: Rs. "
             << fixed << setprecision(2) << subtotal << "\n";
    }

    cout << "----------------------------------------\n";
    cout << "Original Total: Rs. " << total << "\n";
    cout << "Discount: Rs. " << discount << "\n";
    cout << "Final Amount: Rs. " << finalAmount << "\n";
    cout << "Payment Method: " << paymentMethod << "\n";
    cout << "========================================\n";
    cout << "Thank you for ordering from Canteen Buddy!\n";

    // Save order details to orders.txt
    file << "\n========== CANTEEN BUDDY ORDER ==========\n";
    file << "Order ID: " << orderID << "\n";
    file << "Date & Time: " << dateTime << "\n";

    for (const CartItem& item : cart) {
        file << item.food.name
             << " | Qty: " << item.quantity
             << " | Subtotal: Rs. "
             << fixed << setprecision(2)
             << item.food.price * item.quantity << "\n";
    }

    file << "Original Total: Rs. " << total << "\n";
    file << "Discount: Rs. " << discount << "\n";
    file << "Final Amount: Rs. " << finalAmount << "\n";
    file << "Payment Method: " << paymentMethod << "\n";
    file << "========================================\n";

    file.close();

    cart.clear();
    cout << "Order saved successfully!\n";
}

// Admin panel
void adminPanel(vector<Food>& menu) {
    int choice;

    do {
        cout << "\n========== ADMIN PANEL ==========\n";
        cout << "1. View menu\n";
        cout << "2. Add new food\n";
        cout << "3. Update food price\n";
        cout << "4. Delete food item\n";
        cout << "0. Return to main menu\n";

        choice = getIntInput("Enter your choice: ");

        switch (choice) {
            case 1:
                displayMenu(menu);
                break;

            case 2: {
                string name;

                cout << "Enter new food name: ";
                getline(cin, name);

                if (name.empty() || name.find(',') != string::npos) {
                    cout << "Invalid food name. Please try again.\n";
                    break;
                }

                bool exists = false;

                for (const Food& item : menu) {
                    if (item.name == name) {
                        exists = true;
                        break;
                    }
                }

                if (exists) {
                    cout << "This food item already exists.\n";
                    break;
                }

                double price = getDoubleInput("Enter food price: ");

                menu.push_back({name, price});
                saveMenuToFile(menu);

                cout << "Food added successfully!\n";
                break;
            }

            case 3: {
                if (menu.empty()) {
                    cout << "No food items available to update.\n";
                    break;
                }

                displayMenu(menu);

                int number = getIntInput(
                    "Enter food number to update (0 to cancel): ");

                if (number == 0) {
                    break;
                }

                if (number < 1 ||
                    number > static_cast<int>(menu.size())) {
                    cout << "Invalid food number!\n";
                    break;
                }

                double newPrice = getDoubleInput("Enter new price: ");

                menu[number - 1].price = newPrice;
                saveMenuToFile(menu);

                cout << "Price updated successfully!\n";
                break;
            }

            case 4: {
                if (menu.empty()) {
                    cout << "No food items available to delete.\n";
                    break;
                }

                displayMenu(menu);

                int number = getIntInput(
                    "Enter food number to delete (0 to cancel): ");

                if (number == 0) {
                    break;
                }

                if (number < 1 ||
                    number > static_cast<int>(menu.size())) {
                    cout << "Invalid food number!\n";
                    break;
                }

                string deletedName = menu[number - 1].name;

                menu.erase(menu.begin() + (number - 1));
                saveMenuToFile(menu);

                cout << deletedName << " deleted successfully!\n";
                break;
            }

            case 0:
                cout << "Returning to main menu...\n";
                break;

            default:
                cout << "Invalid choice!\n";
        }

    } while (choice != 0);
}

// Main program
int main() {
    vector<Food> menu;
    vector<CartItem> cart;

    loadMenuFromFile(menu);

    int choice;

    do {
        cout << "\n====================================\n";
        cout << "        CANTEEN BUDDY\n";
        cout << "====================================\n";
        cout << "1. Display food menu\n";
        cout << "2. Add food by menu number\n";
        cout << "3. View cart\n";
        cout << "4. Remove food from cart\n";
        cout << "5. Search food and add to cart\n";
        cout << "6. Sort menu by price\n";
        cout << "7. View order history\n";
        cout << "8. Finish order and pay\n";
        cout << "9. Admin Panel\n";
        cout << "0. Exit\n";

        choice = getIntInput("Enter your choice: ");

        switch (choice) {
            case 1:
                displayMenu(menu);
                break;

            case 2:
                addFoodToCart(menu, cart);
                break;

            case 3:
                viewCart(cart);
                break;

            case 4:
                removeFoodFromCart(cart);
                break;

            case 5:
                searchFood(menu, cart);
                break;

            case 6:
                sortMenu(menu);
                break;

            case 7:
                viewOrderHistory();
                break;

            case 8:
                finishOrder(cart);
                break;

            case 9:
                adminPanel(menu);
                break;

            case 0:
                cout << "Thank you for using Canteen Buddy!\n";
                break;

            default:
                cout << "Invalid choice! Please try again.\n";
        }

    } while (choice != 0);

    return 0;
}

