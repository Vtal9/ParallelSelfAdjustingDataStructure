#ifndef DIPLOM_ACTION_H
#define DIPLOM_ACTION_H

enum class ActionType{
    INSERT,
    REMOVE,
    LOOKUP
};

template <typename T>
struct Action {
    T value;
    ActionType actionType;

    Action(T value, ActionType actionType) : value(value), actionType(actionType) {}

    bool operator<(const Action& other) const {
        return value < other.value;
    }
};


#endif //DIPLOM_ACTION_H
