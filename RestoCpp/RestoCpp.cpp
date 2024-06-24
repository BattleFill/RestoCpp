#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <limits>

// Класс продукта
class Product {
public:
    int id;
    std::string name;
    double cost;

    Product(int id, const std::string& name, double cost)
        : id(id), name(name), cost(cost) {}
};

// Класс пункта меню
class MenuItem {
public:
    std::string name;
    std::vector<int> product_ids; // Список ID продуктов, необходимых для приготовления
    std::string description;
    double cost;
    int preparation_time;

    MenuItem(const std::string& name, const std::vector<int>& product_ids,
        const std::string& description, double cost, int preparation_time)
        : name(name), product_ids(product_ids), description(description),
        cost(cost), preparation_time(preparation_time) {}
};

// Класс заказа
class Order {
public:
    enum Status {
        PLACED,
        READY,
        COMPLETED
    };

    int id;
    std::vector<MenuItem> items;
    Status status;
    double totalCost;

    Order(int id) : id(id), status(PLACED), totalCost(0.0) {}

    void addItem(const MenuItem& item) {
        items.push_back(item);
        totalCost += item.cost;
    }
};

// Класс пользователя
class User {
public:
    enum Role {
        ADMIN,
        COOK,
        WAITER,
        GUEST
    };

    std::string firstName;
    std::string lastName;
    std::string middleName;
    std::string login;
    std::string password;
    Role role;

    User(const std::string& firstName, const std::string& lastName, const std::string& middleName,
        const std::string& login, const std::string& password, Role role)
        : firstName(firstName), lastName(lastName), middleName(middleName),
        login(login), password(password), role(role) {}
};

// Функция авторизации
bool authenticate(const std::string& login, const std::string& password, std::vector<User>& users, User& loggedInUser) {
    for (const auto& user : users) {
        if (user.login == login && user.password == password) {
            loggedInUser = user;
            return true;
        }
    }
    return false;
}

// Сохранение данных в файл
void saveData(const std::vector<User>& users, const std::vector<Product>& products, const std::vector<MenuItem>& menuItems) {
    std::ofstream outFile("data.txt");
    // Сохраняем данные пользователей
    outFile << users.size() << "\n";
    for (const auto& user : users) {
        outFile << user.firstName << " " << user.lastName << " " << user.middleName << " "
            << user.login << " " << user.password << " " << user.role << "\n";
    }

    // Сохраняем данные продуктов
    outFile << products.size() << "\n";
    for (const auto& product : products) {
        outFile << product.id << " " << product.name << " " << product.cost << "\n";
    }

    // Сохраняем данные пунктов меню
    outFile << menuItems.size() << "\n";
    for (const auto& menuItem : menuItems) {
        outFile << menuItem.name << "\n";
        outFile << menuItem.product_ids.size() << " ";
        for (int id : menuItem.product_ids) {
            outFile << id << " ";
        }
        outFile << menuItem.description << " " << menuItem.cost << " " << menuItem.preparation_time << "\n";
    }

    outFile.close();
}

// Загрузка данных из файла
void loadData(std::vector<User>& users, std::vector<Product>& products, std::vector<MenuItem>& menuItems) {
    std::ifstream inFile("data.txt");
    if (!inFile.is_open()) return;

    int size;
    inFile >> size;

    // Загружаем данные пользователей
    for (int i = 0; i < size; ++i) {
        std::string firstName, lastName, middleName, login, password;
        int role;
        inFile >> firstName >> lastName >> middleName >> login >> password >> role;
        users.emplace_back(firstName, lastName, middleName, login, password, static_cast<User::Role>(role));
    }

    // Загружаем данные продуктов
    inFile >> size;
    for (int i = 0; i < size; ++i) {
        int id;
        std::string name;
        double cost;
        inFile >> id >> name >> cost;
        products.emplace_back(id, name, cost);
    }

    // Загружаем данные пунктов меню
    inFile >> size;
    for (int i = 0; i < size; ++i) {
        std::string name, description;
        int productSize, preparation_time;
        double cost;
        inFile >> name >> productSize;
        std::vector<int> product_ids(productSize);
        for (int j = 0; j < productSize; ++j) {
            inFile >> product_ids[j];
        }
        inFile >> description >> cost >> preparation_time;
        menuItems.emplace_back(name, product_ids, description, cost, preparation_time);
    }

    inFile.close();

    // Если нет данных пользователей, создаем учетную запись администратора по умолчанию
    if (users.empty()) {
        users.emplace_back("Admin", "Admin", "Admin", "admin", "admin", User::ADMIN);
        std::cout << "Default admin account created. Login: admin, Password: admin\n";
    }

    // Если нет данных продуктов и меню, создаем базовые данные
    if (products.empty()) {
        products.emplace_back(1, "Tomato", 1.0);
        products.emplace_back(2, "Cheese", 1.5);
        products.emplace_back(3, "Dough", 2.0);
        products.emplace_back(4, "Basil", 0.5);
        products.emplace_back(5, "Chicken", 3.0);
        products.emplace_back(6, "Lettuce", 0.7);
        std::cout << "Default products created.\n";
    }

    if (menuItems.empty()) {
        menuItems.emplace_back("Margherita Pizza", std::vector<int>{1, 2, 3, 4}, "Classic pizza with tomato, cheese, dough, and basil.", 8.0, 15);
        menuItems.emplace_back("Chicken Salad", std::vector<int>{5, 6}, "Healthy salad with chicken and lettuce.", 6.0, 10);
        std::cout << "Default menu items created.\n";
    }
}



// Основное меню
void showMainMenu(User& loggedInUser, std::vector<User>& users, std::vector<Product>& products, std::vector<MenuItem>& menuItems, std::vector<Order>& orders) {
    int choice;
    while (true) {
        std::cout << "\n1. View Menu\n2. Place Order\n3. View Orders\n4. Admin Functions\n5. Cook Functions\n6. Waiter Functions\n7. Logout\n";
        std::cin >> choice;

        if (choice == 1) {
            // Просмотр меню
            for (const auto& item : menuItems) {
                std::cout << "Name: " << item.name << ", Description: " << item.description
                    << ", Cost: " << item.cost << ", Preparation Time: " << item.preparation_time << " mins\n";
            }
        }
        else if (choice == 2) {
            // Размещение заказа (только для гостей)
            if (loggedInUser.role == User::GUEST) {
                int orderId = orders.size() + 1;
                Order order(orderId);
                int itemChoice;
                while (true) {
                    std::cout << "Enter menu item number to add to order (0 to finish): ";
                    std::cin >> itemChoice;
                    if (itemChoice == 0) break;
                    if (itemChoice > 0 && itemChoice <= menuItems.size()) {
                        order.addItem(menuItems[itemChoice - 1]);
                    }
                }
                orders.push_back(order);
                std::cout << "Order placed successfully. Total cost: " << order.totalCost << "\n";
                order.status = Order::PLACED;
            }
            else {
                std::cout << "Only guests can place orders.\n";
            }
        }
        else if (choice == 3) {
            // Просмотр заказов
            for (const auto& order : orders) {
                std::cout << "Order ID: " << order.id << ", Status: " << order.status << ", Total Cost: " << order.totalCost << "\n";
                for (const auto& item : order.items) {
                    std::cout << "    Item: " << item.name << ", Cost: " << item.cost << "\n";
                }
            }
        }
        else if (choice == 4) {
            // Функции администратора
            if (loggedInUser.role == User::ADMIN) {
                int adminChoice;
                while (true) {
                    std::cout << "\n1. Add User\n2. Edit User\n3. Delete User\n4. Back\n";
                    std::cin >> adminChoice;
                    if (adminChoice == 1) {
                        std::string firstName, lastName, middleName, login, password;
                        int role;
                        std::cout << "Enter first name: ";
                        std::cin >> firstName;
                        std::cout << "Enter last name: ";
                        std::cin >> lastName;
                        std::cout << "Enter middle name: ";
                        std::cin >> middleName;
                        std::cout << "Enter login: ";
                        std::cin >> login;
                        std::cout << "Enter password: ";
                        std::cin >> password;
                        std::cout << "Enter role (0-Admin, 1-Cook, 2-Waiter, 3-Guest): ";
                        std::cin >> role;
                        users.emplace_back(firstName, lastName, middleName, login, password, static_cast<User::Role>(role));
                    }
                    else if (adminChoice == 2) {
                        // Редактирование пользователя
                        std::string login;
                        std::cout << "Enter login of user to edit: ";
                        std::cin >> login;
                        for (auto& user : users) {
                            if (user.login == login) {
                                std::string firstName, lastName, middleName, password;
                                int role;
                                std::cout << "Enter new first name: ";
                                std::cin >> firstName;
                                std::cout << "Enter new last name: ";
                                std::cin >> lastName;
                                std::cout << "Enter new middle name: ";
                                std::cin >> middleName;
                                std::cout << "Enter new password: ";
                                std::cin >> password;
                                std::cout << "Enter new role (0-Admin, 1-Cook, 2-Waiter, 3-Guest): ";
                                std::cin >> role;
                                user.firstName = firstName;
                                user.lastName = lastName;
                                user.middleName = middleName;
                                user.password = password;
                                user.role = static_cast<User::Role>(role);
                                break;
                            }
                        }
                    }
                    else if (adminChoice == 3) {
                        // Удаление пользователя
                        std::string login;
                        std::cout << "Enter login of user to delete: ";
                        std::cin >> login;
                        for (auto it = users.begin(); it != users.end(); ++it) {
                            if (it->login == login) {
                                users.erase(it);
                                break;
                            }
                        }
                    }
                    else if (adminChoice == 4) {
                        break;
                    }
                }
            }
            else {
                std::cout << "Only administrators can access this menu.\n";
            }
        }
        else if (choice == 5) {
            // Функции повара
            if (loggedInUser.role == User::COOK) {
                for (auto& order : orders) {
                    if (order.status == Order::PLACED) {
                        order.status = Order::READY;
                        std::cout << "Order ID: " << order.id << " is now ready.\n";
                    }
                }
            }
            else {
                std::cout << "Only cooks can access this menu.\n";
            }
        }
        else if (choice == 6) {
            // Функции официанта
            if (loggedInUser.role == User::WAITER) {
                for (auto& order : orders) {
                    if (order.status == Order::READY) {
                        order.status = Order::COMPLETED;
                        std::cout << "Order ID: " << order.id << " is now completed.\n";
                    }
                }
            }
            else {
                std::cout << "Only waiters can access this menu.\n";
            }
        }
        else if (choice == 7) {
            break;
        }
    }
}
int main() {
    std::vector<User> users;
    std::vector<Product> products;
    std::vector<MenuItem> menuItems;
    std::vector<Order> orders;

    loadData(users, products, menuItems);

    User loggedInUser("", "", "", "", "", User::GUEST);
    bool loggedIn = false;
    int choice;

    while (true) {
        std::cout << "1. Login\n2. Enter as Guest\n3. Exit\n";
        std::cin >> choice;

        if (choice == 1) {
            std::string userLogin, password;
            std::cout << "Enter login: ";
            std::cin >> userLogin;
            std::cout << "Enter password: ";
            std::cin >> password;
            loggedIn = authenticate(userLogin, password, users, loggedInUser);
            if (loggedIn) {
                std::cout << "Login successful!\n";
                showMainMenu(loggedInUser, users, products, menuItems, orders);
            }
            else {
                std::cout << "Invalid login or password.\n";
            }
        }
        else if (choice == 2) {
            // Вход как гость
            loggedInUser = User("", "", "", "", "", User::GUEST);
            loggedIn = true;
            std::cout << "Entered as Guest.\n";
            showMainMenu(loggedInUser, users, products, menuItems, orders);
        }
        else if (choice == 3) {
            break;
        }
    }

    saveData(users, products, menuItems);

    return 0;
}