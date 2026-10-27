
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <thread>
#include <limits>
#include <functional>
using namespace std;

mutex outputMutex;

int readInt(const string& prompt, int minimum, int maximum) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= minimum && value <= maximum) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cout << "Invalid entry. Try again.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double readPrice() {
    double value;
    while (true) {
        cout << "Price: $";
        if (cin >> value && value > 0 && value <= 1000000) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cout << "Enter a valid positive price.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string readText(const string& prompt) {
    string value;
    do {
        cout << prompt;
        getline(cin, value);
        if (value.empty())
            cout << "This field cannot be empty.\n";
    } while (value.empty());
    return value;
}

class Product {
private:
    int id;
    string name, description;
    double price;
    int stock;

public:
    Product(int i, string n, string d, double p, int s)
        : id(i), name(n), description(d), price(p), stock(s) {}

    int getId() const { return id; }
    string getName() const { return name; }
    string getDescription() const { return description; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }

    void update(string n, string d, double p, int s) {
        name = n;
        description = d;
        price = p;
        stock = s;
    }

    bool reduceStock(int quantity) {
        if (quantity <= 0 || quantity > stock)
            return false;
        stock -= quantity;
        return true;
    }
};

class Catalog {
private:
    vector<shared_ptr<Product>> products;
    mutable mutex inventoryMutex;
    int nextId = 1;

public:
    void addProduct(const string& name,
                    const string& description,
                    double price, int stock) {
        lock_guard<mutex> lock(inventoryMutex);
        products.push_back(make_shared<Product>(
            nextId++, name, description, price, stock));
    }

    shared_ptr<Product> findProduct(int id) const {
        lock_guard<mutex> lock(inventoryMutex);
        for (const auto& p : products) {
            if (p->getId() == id)
                return p;
        }
        return nullptr;
    }

    void display() const {
        lock_guard<mutex> lock(inventoryMutex);
        cout << "\n--- PRODUCT CATALOG ---\n";

        for (const auto& p : products) {
            cout << "ID " << p->getId()
                 << " | " << p->getName()
                 << " | " << p->getDescription()
                 << " | $" << fixed << setprecision(2)
                 << p->getPrice()
                 << " | Stock: " << p->getStock() << '\n';
        }

        if (products.empty())
            cout << "No products available.\n";
    }

    bool removeProduct(int id) {
        lock_guard<mutex> lock(inventoryMutex);

        for (auto it = products.begin();
             it != products.end(); ++it) {
            if ((*it)->getId() == id) {
                products.erase(it);
                return true;
            }
        }
        return false;
    }

    bool updateProduct(int id, string name,
                       string description,
                       double price, int stock) {
        lock_guard<mutex> lock(inventoryMutex);

        for (const auto& p : products) {
            if (p->getId() == id) {
                p->update(name, description, price, stock);
                return true;
            }
        }
        return false;
    }

    struct OrderLine {
        string name;
        int quantity;
        double price;
    };

    bool purchase(const vector<pair<int, int>>& items,
                  vector<OrderLine>& receipt) {
        lock_guard<mutex> lock(inventoryMutex);

        vector<shared_ptr<Product>> selected;

        for (const auto& item : items) {
            shared_ptr<Product> found;

            for (const auto& p : products) {
                if (p->getId() == item.first) {
                    found = p;
                    break;
                }
            }

            if (!found || item.second <= 0 ||
                found->getStock() < item.second)
                return false;

            selected.push_back(found);
        }

        for (size_t i = 0; i < items.size(); ++i) {
            receipt.push_back({
                selected[i]->getName(),
                items[i].second,
                selected[i]->getPrice()
            });

            selected[i]->reduceStock(items[i].second);
        }

        return true;
    }
};

class ShoppingCart {
private:
    vector<pair<int, int>> items;

public:
    void add(int id, int quantity) {
        if (quantity <= 0)
            return;

        for (auto& item : items) {
            if (item.first == id) {
                item.second += quantity;
                return;
            }
        }

        items.push_back({id, quantity});
    }

    bool remove(int id) {
        for (auto it = items.begin();
             it != items.end(); ++it) {
            if (it->first == id) {
                items.erase(it);
                return true;
            }
        }
        return false;
    }

    bool updateQuantity(int id, int quantity) {
        if (quantity < 0)
            return false;

        if (quantity == 0)
            return remove(id);

        for (auto& item : items) {
            if (item.first == id) {
                item.second = quantity;
                return true;
            }
        }

        return false;
    }

    const vector<pair<int, int>>& getItems() const {
        return items;
    }

    bool empty() const {
        return items.empty();
    }

    void clear() {
        items.clear();
    }

    double total(const Catalog& catalog) const {
        double sum = 0;

        for (const auto& item : items) {
            auto p = catalog.findProduct(item.first);
            if (p)
                sum += p->getPrice() * item.second;
        }

        return sum;
    }

    void display(const Catalog& catalog) const {
        cout << "\n--- YOUR CART ---\n";

        for (const auto& item : items) {
            auto p = catalog.findProduct(item.first);

            if (p) {
                cout << p->getName()
                     << " x " << item.second
                     << " = $" << fixed << setprecision(2)
                     << p->getPrice() * item.second << '\n';
            } else {
                cout << "Product ID " << item.first
                     << " is no longer available.\n";
            }
        }

        cout << "Subtotal: $" << fixed << setprecision(2)
             << total(catalog) << '\n';
    }
};

class User {
private:
    string name, email, shippingAddress;
    ShoppingCart cart;

public:
    User(string n, string e, string a)
        : name(n), email(e), shippingAddress(a) {}

    virtual ~User() = default;

    virtual double discount() const {
        return 0.0;
    }

    virtual string membership() const {
        return "Regular";
    }

    string getName() const { return name; }
    string getEmail() const { return email; }
    string getAddress() const { return shippingAddress; }

    ShoppingCart& getCart() {
        return cart;
    }
};

class PremiumUser : public User {
public:
    using User::User;

    double discount() const override {
        return 0.10;
    }

    string membership() const override {
        return "Premium";
    }
};

class VIPUser : public User {
public:
    using User::User;

    double discount() const override {
        return 0.20;
    }

    string membership() const override {
        return "VIP";
    }
};

bool checkout(User& user, Catalog& catalog,
              bool printReceipt = true) {
    if (user.getCart().empty()) {
        if (printReceipt)
            cout << "Your cart is empty.\n";
        return false;
    }

    vector<Catalog::OrderLine> receipt;

    if (!catalog.purchase(user.getCart().getItems(), receipt)) {
        if (printReceipt)
            cout << "Purchase failed: insufficient stock "
                 << "or unavailable product.\n";
        return false;
    }

    double subtotal = 0;

    for (const auto& line : receipt)
        subtotal += line.price * line.quantity;

    double savings = subtotal * user.discount();

    if (printReceipt) {
        cout << "\n======= ORDER RECEIPT =======\n";
        cout << "Customer: " << user.getName()
             << " (" << user.membership() << ")\n";
        cout << "Email: " << user.getEmail() << '\n';
        cout << "Ship to: " << user.getAddress() << '\n';

        for (const auto& line : receipt) {
            cout << line.name
                 << " x " << line.quantity
                 << " @ $" << fixed << setprecision(2)
                 << line.price
                 << " = $" << line.price * line.quantity
                 << '\n';
        }

        cout << "Subtotal: $" << subtotal
             << "\nDiscount: -$" << savings
             << "\nTotal paid (simulated): $"
             << subtotal - savings << '\n';

        cout << "Purchase successful!\n";
        cout << "=============================\n";
    }

    user.getCart().clear();
    return true;
}

void concurrencyDemo() {
    Catalog demo;

    demo.addProduct(
        "Limited Edition Headphones",
        "Only 3 available",
        49.99,
        3
    );

    User customer1(
        "Customer A", "a@example.com", "Demo address A"
    );

    VIPUser customer2(
        "Customer B", "b@example.com", "Demo address B"
    );

    auto worker = [&demo](User& customer) {
        customer.getCart().add(1, 2);

        bool success = checkout(customer, demo, false);

        lock_guard<mutex> lock(outputMutex);

        cout << customer.getName()
             << (success
                 ? " purchased 2 items.\n"
                 : " could not purchase 2 items "
                   "(insufficient stock).\n");
    };

    cout << "\nTwo threads attempt to buy 2 units each; "
         << "only 3 exist.\n";

    thread first(worker, ref(customer1));
    thread second(worker, ref(customer2));

    first.join();
    second.join();

    demo.display();

    cout << "Only one purchase can succeed; "
         << "inventory never becomes negative.\n";
}

int main() {
    Catalog catalog;

    catalog.addProduct(
        "Laptop", "15-inch laptop", 799.99, 5
    );
    catalog.addProduct(
        "Headphones", "Wireless headphones", 59.99, 10
    );
    catalog.addProduct(
        "Keyboard", "Mechanical keyboard", 89.99, 8
    );
    catalog.addProduct(
        "Mouse", "Wireless mouse", 24.99, 12
    );

    cout << "WELCOME TO SMART SHOPPING SYSTEM\n";

    string name = readText("Your name: ");

    string email;

    do {
        email = readText("Your email: ");

        if (email.find('@') == string::npos ||
            email.find('.') == string::npos) {
            cout << "Enter a valid email address.\n";
        } else {
            break;
        }
    } while (true);

    string address = readText("Shipping address: ");

    cout << "Membership: 1. Regular  "
         << "2. Premium (10% off)  "
         << "3. VIP (20% off)\n";

    int type = readInt("Choose membership: ", 1, 3);

    unique_ptr<User> user;

    if (type == 2) {
        user.reset(new PremiumUser(name, email, address));
    } else if (type == 3) {
        user.reset(new VIPUser(name, email, address));
    } else {
        user.reset(new User(name, email, address));
    }

    while (true) {
        cout << "\n1. Browse products\n"
             << "2. Add to cart\n"
             << "3. View cart\n"
             << "4. Remove from cart\n"
             << "5. Change cart quantity\n"
             << "6. Checkout\n"
             << "7. Add catalog product\n"
             << "8. Update catalog product\n"
             << "9. Remove catalog product\n"
             << "10. Run multithreading demonstration\n"
             << "0. Exit\n";

        int choice = readInt("Choose an option: ", 0, 10);

        if (choice == 0) {
            cout << "Thank you for shopping!\n";
            break;
        }

        if (choice == 1) {
            catalog.display();
        }
        else if (choice == 2) {
            catalog.display();

            int id = readInt("Product ID: ", 1, 1000000);
            auto p = catalog.findProduct(id);

            if (!p) {
                cout << "Product not found.\n";
                continue;
            }

            int quantity = readInt("Quantity: ", 1, 1000000);
            int existing = 0;

            for (const auto& item :
                 user->getCart().getItems()) {
                if (item.first == id)
                    existing = item.second;
            }

            if (quantity > p->getStock() - existing) {
                cout << "Not enough available stock.\n";
            } else {
                user->getCart().add(id, quantity);
                cout << "Added to cart.\n";
            }
        }
        else if (choice == 3) {
            user->getCart().display(catalog);
        }
        else if (choice == 4) {
            int id = readInt(
                "Product ID to remove: ", 1, 1000000
            );

            cout << (user->getCart().remove(id)
                     ? "Removed.\n"
                     : "Not in cart.\n");
        }
        else if (choice == 5) {
            int id = readInt(
                "Product ID to change: ", 1, 1000000
            );

            int quantity = readInt(
                "New quantity (0 removes): ", 0, 1000000
            );

            auto p = catalog.findProduct(id);

            if (quantity > 0 &&
                (!p || quantity > p->getStock())) {
                cout << "Invalid product or "
                     << "insufficient stock.\n";
            } else {
                cout << (user->getCart().updateQuantity(
                             id, quantity)
                         ? "Cart updated.\n"
                         : "Product not in cart.\n");
            }
        }
        else if (choice == 6) {
            user->getCart().display(catalog);

            string answer = readText(
                "Complete purchase? (yes/no): "
            );

            if (answer == "yes" ||
                answer == "YES" ||
                answer == "Yes") {
                checkout(*user, catalog);
            } else {
                cout << "Checkout canceled.\n";
            }
        }
        else if (choice == 7) {
            string n = readText("Product name: ");
            string d = readText("Description: ");
            double price = readPrice();
            int stock = readInt("Stock: ", 0, 1000000);

            catalog.addProduct(n, d, price, stock);
            cout << "Product added.\n";
        }
        else if (choice == 8) {
            int id = readInt(
                "Product ID to update: ", 1, 1000000
            );

            if (!catalog.findProduct(id)) {
                cout << "Product not found.\n";
                continue;
            }

            string n = readText("New name: ");
            string d = readText("New description: ");
            double price = readPrice();
            int stock = readInt("New stock: ", 0, 1000000);

            cout << (catalog.updateProduct(
                         id, n, d, price, stock)
                     ? "Product updated.\n"
                     : "Product not found.\n");
        }
        else if (choice == 9) {
            int id = readInt(
                "Product ID to remove: ", 1, 1000000
            );

            cout << (catalog.removeProduct(id)
                     ? "Product removed.\n"
                     : "Product not found.\n");
        }
        else if (choice == 10) {
            concurrencyDemo();
        }
    }

    return 0;
}
