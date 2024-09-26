#include <gtk/gtk.h>
#include <string>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <cctype>

// 判断操作符的优先级
int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

// 应用操作符进行计算
double apply_operator(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        default: throw std::runtime_error("Invalid operator");
    }
}

// 解析并计算表达式
double evaluate_expression(const std::string &expr) {
    std::stack<double> values;  // 存储操作数
    std::stack<char> ops;       // 存储操作符

    for (size_t i = 0; i < expr.length(); i++) {
        // 跳过空格
        if (isspace(expr[i])) continue;

        // 处理数字
        if (isdigit(expr[i])) {
            double val = 0;
            while (i < expr.length() && (isdigit(expr[i]) || expr[i] == '.')) {
                if (expr[i] == '.') {
                    i++;
                    double fraction = 1;
                    while (i < expr.length() && isdigit(expr[i])) {
                        fraction /= 10;
                        val += (expr[i] - '0') * fraction;
                        i++;
                    }
                    break;
                }
                val = val * 10 + (expr[i] - '0');
                i++;
            }
            values.push(val);
            i--;  // 回到最后处理的数字
        }
        // 处理操作符
        else {
            while (!ops.empty() && precedence(ops.top()) >= precedence(expr[i])) {
                double b = values.top();
                values.pop();

                double a = values.top();
                values.pop();

                char op = ops.top();
                ops.pop();

                values.push(apply_operator(a, b, op));
            }
            ops.push(expr[i]);
        }
    }

    // 处理剩下的操作符
    while (!ops.empty()) {
        double b = values.top();
        values.pop();

        double a = values.top();
        values.pop();

        char op = ops.top();
        ops.pop();

        values.push(apply_operator(a, b, op));
    }

    return values.top();
}

static std::string expression;

static void button_clicked(GtkWidget *widget, gpointer data) {
    const char *button_label = gtk_button_get_label(GTK_BUTTON(widget));

    if (g_strcmp0(button_label, "C") == 0) {
        expression.clear();
        gtk_entry_set_text(GTK_ENTRY(data), "");
    } else if (g_strcmp0(button_label, "=") == 0) {
        // 计算表达式
        try {
            double result = evaluate_expression(expression);
            gtk_entry_set_text(GTK_ENTRY(data), std::to_string(result).c_str());
            expression.clear();
        } catch (const std::invalid_argument &) {
            gtk_entry_set_text(GTK_ENTRY(data), "Error");
            expression.clear();
        } catch (const std::runtime_error &) {
            gtk_entry_set_text(GTK_ENTRY(data), "Error");
            expression.clear();
        }
    } else {
        // 将按键的字符添加到表达式中
        expression += button_label;
        gtk_entry_set_text(GTK_ENTRY(data), expression.c_str());
    }
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *entry;
    GtkWidget *buttons[16];
    const char *labels[16] = {"7", "8", "9", "/",
                              "4", "5", "6", "*",
                              "1", "2", "3", "-",
                              "0", "C", "=", "+"};

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 250);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 0, 4, 1);

    for (int i = 0; i < 16; i++) {
        buttons[i] = gtk_button_new_with_label(labels[i]);
        gtk_grid_attach(GTK_GRID(grid), buttons[i], i % 4, (i / 4) + 1, 1, 1);
        g_signal_connect(buttons[i], "clicked", G_CALLBACK(button_clicked), entry);
    }

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
