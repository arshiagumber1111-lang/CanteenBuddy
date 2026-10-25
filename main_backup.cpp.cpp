
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <cctype>
#include <limits>
#include <algorithm>
using namespace std;

struct Food {
    string name;
    double price;
};

struct CartItem {
    string name;
    double price;
    int quantity;
    double subtotal;
};

struct Order {
    int orderId = 0;
    string dateTime;
    vector<CartItem> items;
    double originalTotal = 0;
    double discount = 0;
    double finalTotal = 0;
    string paymentMethod;
};

string toLowerCase(string s) {
    for (char& c : s)
        c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return s;
}

string getDateTime() {
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    if (!t) return "Date unavailable";

    char buffer[30];
    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", t);
    return buffer;
}

void displayMenu(const vector<Food>& menu) {
    cout << "\n========== CANTEEN MENU ==========\n";
    cout << left << setw(8) << "Number"
         << setw(20) << "Food Item" << "Price\n";

    for (size_t i = 0; i < menu.size(); i++) {
        cout << left << setw(8) << i + 1
             << setw(20) << menu[i].name
             << "Rs. " << fixed << setprecision(2)
             << menu[i].price << '\n';
    }
    cout << "==================================\n";
}

void sortMenuByPrice(vector<Food>& menu) {
    for (size_t i = 0; i < menu.size(); i++) {
        for (size_t j = 0; j + 1 < menu.size() - i; j++) {
            if (menu[j].price > menu[j + 1].price)
                swap(menu[j], menu[j + 1]);
        }
    }
}

int searchFood(const vector<Food>& menu, const string& name) {
    for (size_t i = 0; i < menu.size(); i++) {
        if (toLowerCase(menu[i].name) == toLowerCase(name))
            return static_cast<int>(i);
    }
    return -1;
}

void addToCart(vector<CartItem>& cart, const Food& food, int quantity) {
    if (quantity <= 0) {
        cout << "Quantity must be greater than zero.\n";
        return;
    }

    for (auto& item : cart) {
        if (toLowerCase(item.name) == toLowerCase(food.name)) {
            item.quantity += quantity;
            item.subtotal = item.price * item.quantity;
            cout << food.name << " quantity updated in cart.\n";
            return;
        }
    }

    cart.push_back({food.name, food.price, quantity,
                    food.price * quantity});
    cout << food.name << " added to cart successfully.\n";
}

void viewCart(const vector<CartItem>& cart) {
    cout << "\n============= YOUR CART =============\n";

    if (cart.empty()) {
        cout << "Your cart is empty.\n";
        return;
    }

    double total = 0;
    for (const auto& item : cart) {
        cout << item.name << " x " << item.quantity
             << " = Rs. " << item.subtotal << '\n';
        total += item.subtotal;
    }
    cout << "Grand Total: Rs. " << total << '\n';
}

void removeFromCart(vector<CartItem>& cart) {
    if (cart.empty()) {
        cout << "Your cart is empty.\n";
        return;
    }

    viewCart(cart);
    string name;
    cout << "Enter food name to remove: ";
    getline(cin, name);

    int index = -1;
    for (size_t i = 0; i < cart.size(); i++) {
        if (toLowerCase(cart[i].name) == toLowerCase(name)) {
            index = static_cast<int>(i);
            break;
        }
    }

    if (index == -1) {
        cout << "Food item not found in cart.\n";
        return;
    }

    int quantity;
    cout << "Enter quantity to remove: ";
    if (!(cin >> quantity)) {
        cout << "Invalid quantity.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (quantity <= 0) {
        cout << "Quantity must be positive.\n";
    } else if (quantity >= cart[index].quantity) {
        cout << cart[index].name << " removed from cart.\n";
        cart.erase(cart.begin() + index);
    } else {
        cart[index].quantity -= quantity;
        cart[index].subtotal = cart[index].price * cart[index].quantity;
        cout << "Quantity removed successfully.\n";
    }
}

void searchAndAdd(const vector<Food>& menu, vector<CartItem>& cart) {
    string name;
    cout << "Enter food name to search: ";
    getline(cin, name);

    int index = searchFood(menu, name);
    if (index == -1) {
        cout << "Food item not found.\n";
        return;
    }

    cout << menu[index].name << " costs Rs. "
         << menu[index].price << '\n';

    int quantity;
    cout << "Enter quantity: ";
    if (!(cin >> quantity)) {
        cout << "Invalid quantity.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    addToCart(cart, menu[index], quantity);
}

// ---------------- ADMIN PANEL ----------------

void adminPanel(vector<Food>& menu) {
    int choice;

    do {
        cout << "\n========== ADMIN PANEL ==========\n";
        cout << "1. View menu\n";
        cout << "2. Add new food\n";
        cout << "3. Update food price\n";
        cout << "4. Delete food item\n";
        cout << "0. Return to main menu\n";
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cout << "Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 1) {
            displayMenu(menu);
        }
        else if (choice == 2) {
            string name;
            double price;

            cout << "Enter new food name: ";
            getline(cin, name);

            if (name.empty() || searchFood(menu, name) != -1) {
                cout << "Name is empty or food already exists.\n";
                continue;
            }

            cout << "Enter price in Rs.: ";
            if (!(cin >> price)) {
                cout << "Invalid price.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (price <= 0) {
                cout << "Price must be greater than zero.\n";
                continue;
            }

            menu.push_back({name, price});
            cout << "Food added successfully.\n";
        }
        else if (choice == 3) {
            displayMenu(menu);

            string name;
            cout << "Enter food name to update: ";
            getline(cin, name);

            int index = searchFood(menu, name);
            if (index == -1) {
                cout << "Food item not found.\n";
                continue;
            }

            double newPrice;
            cout << "Enter new price: ";
            if (!(cin >> newPrice)) {
                cout << "Invalid price.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (newPrice <= 0) {
                cout << "Price must be greater than zero.\n";
                continue;
            }

            menu[index].price = newPrice;
            cout << "Price updated successfully.\n";
        }
        else if (choice == 4) {
            displayMenu(menu);

            string name;
            cout << "Enter food name to delete: ";
            getline(cin, name);

            int index = searchFood(menu, name);
            if (index == -1) {
                cout << "Food item not found.\n";
                continue;
            }

            menu.erase(menu.begin() + index);
            cout << "Food deleted successfully.\n";
        }
        else if (choice != 0) {
            cout << "Invalid choice.\n";
        }

    } while (choice != 0);
}

// ---------------- FILE HANDLING ----------------

void saveOrderToFile(const Order& order) {
    ofstream file("orders.txt", ios::app);

    if (!file) {
        cout << "Error: Could not save order to file!\n";
        return;
    }

    file << fixed << setprecision(2);
    file << "ORDER\n";
    file << "Order ID: " << order.orderId << '\n';
    file << "Date & Time: " << order.dateTime << '\n';

    for (const auto& item : order.items) {
        file << item.name << " x " << item.quantity
             << " = Rs. " << item.subtotal << '\n';
    }

    file << "Original Total: Rs. " << order.originalTotal << '\n';
    file << "Discount: Rs. " << order.discount << '\n';
    file << "Final Amount: Rs. " << order.finalTotal << '\n';
    file << "Payment Method: " << order.paymentMethod << '\n';
    file << "--------------------------------------\n";
}

void loadOrderHistory(vector<Order>& history) {
    ifstream file("orders.txt");
    if (!file) {
        cout << "No previous order history found.\n";
        return;
    }

    string line;
    Order order;
    bool readingOrder = false;

    while (getline(file, line)) {
        if (line == "ORDER") {
            if (readingOrder)
                history.push_back(order);

            order = Order{};
            readingOrder = true;
        }
        else if (!readingOrder) {
            continue;
        }
        else if (line.find("Order ID: ") == 0) {
            try { order.orderId = stoi(line.substr(10)); }
            catch (...) { order.orderId = 0; }
        }
        else if (line.find("Date & Time: ") == 0) {
            order.dateTime = line.substr(13);
        }
        else if (line.find("Original Total: Rs. ") == 0) {
            try { order.originalTotal = stod(line.substr(20)); }
            catch (...) { order.originalTotal = 0; }
        }
        else if (line.find("Discount: Rs. ") == 0) {
            try { order.discount = stod(line.substr(14)); }
            catch (...) { order.discount = 0; }
        }
        else if (line.find("Final Amount: Rs. ") == 0) {
            try { order.finalTotal = stod(line.substr(18)); }
            catch (...) { order.finalTotal = 0; }
        }
        else if (line.find("Payment Method: ") == 0) {
            order.paymentMethod = line.substr(16);
        }
        else if (line.find(" = Rs. ") != string::npos) {
            size_t xPos = line.find(" x ");
            size_t totalPos = line.find(" = Rs. ");

            if (xPos != string::npos && totalPos != string::npos &&
                xPos < totalPos) {
                try {
                    CartItem item;
                    item.name = line.substr(0, xPos);
                    item.quantity = stoi(
                        line.substr(xPos + 3, totalPos - xPos - 3)
                    );
                    item.subtotal = stod(line.substr(totalPos + 7));
                    item.price = item.quantity > 0
                        ? item.subtotal / item.quantity : 0;
                    order.items.push_back(item);
                } catch (...) {
                    // Ignore malformed item lines.
                }
            }
        }
    }

    if (readingOrder)
        history.push_back(order);

    cout << "Previous orders loaded: " << history.size() << '\n';
}

void viewOrderHistory(const vector<Order>& history) {
    cout << "\n========== ORDER HISTORY ==========\n";

    if (history.empty()) {
        cout << "No completed orders found.\n";
        return;
    }

    for (size_t i = 0; i < history.size(); i++) {
        const Order& order = history[i];

        cout << "\nOrder " << i + 1 << '\n';
        cout << "Order ID: " << order.orderId << '\n';
        cout << "Date & Time: " << order.dateTime << '\n';

        for (const auto& item : order.items) {
            cout << "  " << item.name << " x " << item.quantity
                 << " = Rs. " << item.subtotal << '\n';
        }

        cout << "Original Total: Rs. " << order.originalTotal << '\n';
        cout << "Discount: Rs. " << order.discount << '\n';
        cout << "Final Amount: Rs. " << order.finalTotal << '\n';
        cout << "Payment Method: " << order.paymentMethod << '\n';
        cout << "-----------------------------------\n";
    }
}

void printBill(const Order& order) {
    cout << "\n========== CANTEEN BUDDY BILL ==========\n";
    cout << "Order ID: " << order.orderId << '\n';
    cout << "Date & Time: " << order.dateTime << '\n';
    cout << "----------------------------------------\n";

    for (const auto& item : order.items) {
        cout << item.name << " x " << item.quantity
             << " = Rs. " << item.subtotal << '\n';
    }

    cout << "Original Total: Rs. " << order.originalTotal << '\n';
    cout << "Discount: Rs. " << order.discount << '\n';
    cout << "Final Amount: Rs. " << order.finalTotal << '\n';
    cout << "Payment Method: " << order.paymentMethod << '\n';
    cout << "========================================\n";
    cout << "Thank you for visiting Canteen Buddy!\n";
}

void checkout(vector<CartItem>& cart, vector<Order>& history) {
    if (cart.empty()) {
        cout << "Your cart is empty. Add food first.\n";
        return;
    }

    double total = 0;
    for (const auto& item : cart)
        total += item.subtotal;

    double discount = (total >= 500) ? total * 0.10 : 0;

    cout << "\nOriginal Total: Rs. " << total << '\n';
    cout << "Discount: Rs. " << discount << '\n';
    cout << "Final Amount: Rs. " << total - discount << '\n';

    cout << "\n1. Cash\n2. UPI\nChoose payment method: ";

    int paymentChoice;
    if (!(cin >> paymentChoice)) {
        cout << "Invalid payment choice.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string payment;
    if (paymentChoice == 1)
        payment = "Cash";
    else if (paymentChoice == 2)
        payment = "UPI";
    else {
        cout << "Invalid payment choice.\n";
        return;
    }

    time_t now = time(nullptr);

    Order order;
    order.orderId = static_cast<int>(now % 1000000);
    order.dateTime = getDateTime();
    order.items = cart;
    order.originalTotal = total;
    order.discount = discount;
    order.finalTotal = total - discount;
    order.paymentMethod = payment;

    history.push_back(order);
    saveOrderToFile(order);
    cout << "Order saved to orders.txt.\n";

    printBill(order);
    cart.clear();
}

// ---------------- MAIN PROGRAM ----------------

int main() {
    vector<Food> menu = {
        {"Maggi", 40},
        {"Sandwich", 50},
        {"Burger", 60},
        {"Cold Coffee", 70},
        {"Pizza", 120}
    };

    vector<CartItem> cart;
    vector<Order> history;

    loadOrderHistory(history);
    sortMenuByPrice(menu);

    int choice;

    do {
        cout << "\n=============== CANTEEN BUDDY ===============\n";
        cout << "1. Display food menu\n";
        cout << "2. Add food by menu number\n";
        cout << "3. View cart\n";
        cout << "4. Remove food from cart\n";
        cout << "5. Search food and add to cart\n";
        cout << "6. Sort menu by price\n";
        cout << "7. View order history (all saved orders)\n";
        cout << "8. Finish order and pay\n";
        cout << "9. Admin Panel\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cout << "Please enter a valid number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                displayMenu(menu);
                break;

            case 2: {
                displayMenu(menu);
                int number, quantity;

                cout << "Enter food number: ";
                if (!(cin >> number)) {
                    cout << "Invalid food number.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }

                if (number < 1 || number > static_cast<int>(menu.size())) {
                    cout << "Invalid food number.\n";
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }

                cout << "Enter quantity: ";
                if (!(cin >> quantity)) {
                    cout << "Invalid quantity.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                addToCart(cart, menu[number - 1], quantity);
                break;
            }

            case 3:
                viewCart(cart);
                break;

            case 4:
                removeFromCart(cart);
                break;

            case 5:
                searchAndAdd(menu, cart);
                break;

            case 6:
                sortMenuByPrice(menu);
                displayMenu(menu);
                break;

            case 7:
                viewOrderHistory(history);
                break;

            case 8:
                checkout(cart, history);
                break;

            case 9:
                adminPanel(menu);
                break;

            case 0:
                cout << "Thank you for using Canteen Buddy!\n";
                break;

            default:
                cout << "Invalid choice. Please try again.\n";
        }

    } while (choice != 0);

    return 0;
}


