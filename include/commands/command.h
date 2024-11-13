#ifndef COMMAND_H
#define COMMAND_H

#include <dpp/appcommand.h>
#include <dpp/dpp.h>
#include <string>
#include <globals.h>

class Command {
public:
    Command(std::string name, std::string description) : name(name), description(description) {}
    virtual ~Command() = default;
    virtual void execute(const dpp::slashcommand_t& event, dpp::cluster&bot) = 0;

    void register_command(dpp::cluster& bot) {
        dpp::slashcommand command(name, description, bot.me.id);
        for (auto option : options) {
            command.add_option(option);
        }
        bot.global_command_create(command, [this](const dpp::confirmation_callback_t& callback) {
            if (callback.is_error()) {
                std::cerr << "Failed to register command " + name + " command: " << callback.get_error().message << std::endl;
            }
        });
    }

    std::string get_name() const { return name; }

protected:
    std::string name;
    std::string description;
    std::vector<dpp::command_option> options;
    void add_option(dpp::command_option option) {
        options.push_back(option);
    }
};

#endif
