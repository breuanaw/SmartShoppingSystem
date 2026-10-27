# Smart Shopping System

#### Video Demo: (https://youtu.be/0Mg9oh6cZaA)

#### Description:
Project Overview

Smart Shopping System is a console-based online shopping application developed using C++. The purpose of this project is to demonstrate how programming concepts can be used to build a functional shopping system. The application allows customers to browse products, add items to their shopping carts, update quantities, remove products, and complete purchases.

The program also includes inventory management, three customer membership levels, discount calculations, input validation, and multithreading. I designed this project to apply the object-oriented programming and concurrency concepts I learned throughout my C++ course.

Product Management

The Product class stores information about each product, including its identification number, name, description, price, and stock quantity. The Catalog class manages the products available for purchase.

The catalog includes laptops, headphones, keyboards, and computer mice. Users can browse these products and view their prices and available quantities.

The application also allows products to be added, updated, and removed. I included these functions because inventory management is an important part of an online shopping system.

Shopping Cart

The ShoppingCart class allows customers to add products to their carts, remove products, and change quantities. It also calculates the subtotal based on the selected products and their prices.

I created a separate ShoppingCart class to keep cart operations organized instead of placing every function inside the main program. This makes the code easier to read and maintain.

User Accounts and Memberships

The User class stores customer information, including the customer's name, email address, shipping address, and shopping cart.

The application includes three membership types:

Regular User: No discount.

Premium User: 10% discount.

VIP User: 20% discount.

I used inheritance to create PremiumUser and VIPUser classes based on the User class. Polymorphism allows the program to calculate different discounts depending on the customer's membership type.

This approach reduces repetitive code and makes it easier to add additional membership levels in the future.

Checkout and Payment Simulation

The checkout feature allows customers to review their carts and confirm their purchases by entering "yes."

Before completing a purchase, the application checks whether the requested products are available. If sufficient stock exists, the program updates the inventory, applies the customer's membership discount, and displays an order receipt.

The receipt includes customer information, purchased products, quantities, subtotal, discount, and final purchase total.

The payment system is simulated and does not process actual credit card transactions. I chose this design because the project's primary focus is C++ programming rather than integration with a real payment processor.

Multithreading

One of the most important features of Smart Shopping System is multithreading.

The application uses C++ threads to simulate two customers attempting to purchase the same product simultaneously.

During the demonstration, a product has three available units, and two customers each attempt to purchase two units.

Because there are not enough products for both customers, only one purchase can succeed.

I implemented a mutex to protect shared inventory during transactions. This prevents race conditions and ensures that stock quantities remain accurate.

Multithreading is important in real-world shopping applications because multiple customers may attempt to purchase limited-stock products at the same time.

Error Handling and Validation

The application includes input validation to reduce errors and prevent invalid transactions.

For example, the program rejects invalid menu selections, negative quantities, and invalid prices. It also checks whether products exist and whether sufficient inventory is available before completing purchases.

These validation techniques help improve the application's reliability and overall user experience.

Project Files

main.cpp

This file contains the complete C++ source code for the shopping system. It includes the Product, Catalog, ShoppingCart, User, PremiumUser, and VIPUser classes.

It also contains the program's main menu, inventory management functions, checkout process, input validation, and multithreading demonstration.

README.md

This file contains the project's documentation, including its purpose, functionality, design decisions, file descriptions, and instructions for running the application. It also provides the demonstration video link.

Design Decisions

I chose object-oriented programming because it allows the application to be divided into smaller components with specific responsibilities.

Encapsulation protects class attributes by restricting direct access to internal data. Inheritance allows different user types to share common functionality, while polymorphism supports different membership discounts.

I used smart pointers to help manage dynamically allocated objects and reduce the risk of memory management errors.

I also chose a console-based interface because it allowed me to focus on the program's functionality and demonstrate the programming concepts covered throughout the course.

How to Run the Application

Download the main.cpp file from this GitHub repository.

Open Code::Blocks or another C++ development environment.

Create a C++ console application.

Add the main.cpp source code to the project.

Enable the C++17 language standard.

Build and run the application.

Enter customer information and select a membership type.

Use the menu to browse products, manage the shopping cart, complete purchases, and demonstrate multithreading.

Conclusion

Developing Smart Shopping System allowed me to apply important C++ concepts in a practical application.

Through this project, I gained experience with object-oriented programming, inheritance, polymorphism, inventory management, input validation, and multithreading.

The project also demonstrated the importance of protecting shared resources when multiple operations occur simultaneously.

These skills will be useful as I continue studying information technology and developing more advanced software applications.
