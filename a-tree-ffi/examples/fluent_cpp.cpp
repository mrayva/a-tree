// Example demonstrating fluent API and simplified error handling
//
// This shows:
// 1. Fluent tree construction with Tree::builder()
// 2. No need for .unwrap() - methods throw by default
// 3. try_* variants available for explicit Result-based error handling

#include <iostream>
#include "../atree.hpp"

using namespace atree;

int main() {
    try {
        std::cout << "=== Fluent Tree Construction ===\n\n";

        // Option 1: Fluent builder pattern (NEW!)
        auto tree1 = std::move(Tree::builder()
            .with_integer("user_id")
            .with_float("price")
            .with_boolean("is_active"))
            .build();

        std::cout << "✓ Created tree using fluent builder\n\n";

        // Option 2: Traditional initializer list (still works)
        Tree tree2({
            AttributeDefinition::integer("user_id"),
            AttributeDefinition::float_attr("price"),
            AttributeDefinition::boolean("is_active")
        });

        std::cout << "✓ Created tree using initializer list\n\n";

        // ====================================================================
        std::cout << "=== Simplified Error Handling ===\n\n";

        // No .unwrap() needed! Methods throw by default
        tree1.insert(1, "user_id > 100 and price < 50.0 and is_active");
        std::cout << "✓ Inserted expression (no .unwrap() needed!)\n";

        // Search - no .unwrap() needed either!
        auto matches = tree1.search(
            tree1.make_event()
                .with_integer("user_id", 150)
                .with_float("price", 45.99)
                .with_boolean("is_active", true)
        );

        std::cout << "✓ Found " << matches.size() << " matches (no .unwrap()!)\n\n";

        // Export graphviz - still no .unwrap()!
        auto dot = tree1.to_graphviz();
        std::cout << "✓ Generated Graphviz (" << dot.length() << " bytes)\n\n";

        // ====================================================================
        std::cout << "=== Result-Based Error Handling (Optional) ===\n\n";

        // If you want explicit error handling, use try_* methods
        auto insert_result = tree2.try_insert(2, "invalid expression!");
        if (insert_result.is_err()) {
            std::cout << "✓ Caught error with Result: " << insert_result.error() << "\n\n";
        }

        // try_search returns Result<vector<uint64_t>>
        auto search_result = tree2.try_search(
            tree2.make_event()
                .with_integer("user_id", 200)
                .with_float("price", 30.0)
                .with_boolean("is_active", false)
        );

        if (search_result.is_ok()) {
            std::cout << "✓ Search returned Result with "
                      << search_result.unwrap().size() << " matches\n\n";
        }

        // ====================================================================
        std::cout << "=== Comparison ===\n\n";

        std::cout << "OLD WAY (verbose):\n";
        std::cout << "  auto result = tree.insert(1, expr);\n";
        std::cout << "  if (result.is_err()) { ... }\n";
        std::cout << "  auto matches = tree.search(builder).unwrap();\n\n";

        std::cout << "NEW WAY (clean):\n";
        std::cout << "  tree.insert(1, expr);  // throws on error\n";
        std::cout << "  auto matches = tree.search(builder);  // no unwrap!\n\n";

        std::cout << "RESULT WAY (explicit):\n";
        std::cout << "  auto result = tree.try_insert(1, expr);\n";
        std::cout << "  if (result.is_err()) { ... }\n\n";

        // ====================================================================
        std::cout << "=== Complete Example ===\n\n";

        // Fluent everything, no unwrap needed!
        auto tree = std::move(Tree::builder()
            .with_boolean("premium")
            .with_integer("age")
            .with_string("country"))
            .build();

        tree.insert(1, "premium and age >= 18 and country = \"US\"");
        tree.insert(2, "age >= 21");

        auto results = tree.search(
            tree.make_event()
                .with_boolean("premium", true)
                .with_integer("age", 25)
                .with_string("country", "US")
        );

        std::cout << "Found " << results.size() << " matching subscriptions: ";
        for (auto id : results) {
            std::cout << id << " ";
        }
        std::cout << "\n\n";

        std::cout << "✓ All operations completed successfully!\n";
        std::cout << "\nKey improvements:\n";
        std::cout << "  1. Tree::builder() for fluent construction\n";
        std::cout << "  2. Methods throw by default (no .unwrap())\n";
        std::cout << "  3. try_* methods available for Result-based handling\n";
        std::cout << "  4. Choose exception-based OR Result-based style\n";

        return 0;

    } catch (const Error& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
