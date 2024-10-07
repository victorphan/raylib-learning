#include <cassert>
#include <iostream>
#include <map>
#include <optional>

namespace tetris {

enum class BaseActionScore {
    Single,
    Double,
    Triple,
    Tetris,
    MiniTSpinZero,
    TSpinZero,
    MiniTSpinSingle,
    TSpinSingle,
    MiniTSpinDouble,
    TSpinDouble,
    TSpinTriple
};

const auto action_to_score =
    std::map<BaseActionScore, int>{{BaseActionScore::Single, 100},          {BaseActionScore::Double, 300},
                                   {BaseActionScore::Triple, 500},          {BaseActionScore::Tetris, 800},
                                   {BaseActionScore::MiniTSpinZero, 100},   {BaseActionScore::TSpinZero, 400},
                                   {BaseActionScore::MiniTSpinSingle, 200}, {BaseActionScore::TSpinSingle, 800},
                                   {BaseActionScore::MiniTSpinDouble, 400}, {BaseActionScore::TSpinDouble, 1200},
                                   {BaseActionScore::TSpinTriple, 1600}};

enum class TSpinType { NotTSpin, WallKick, NoWallKick };

inline auto isDifficult(BaseActionScore base_action_score) -> bool {
    return base_action_score == BaseActionScore::Tetris || base_action_score == BaseActionScore::MiniTSpinSingle ||
           base_action_score == BaseActionScore::TSpinSingle || base_action_score == BaseActionScore::MiniTSpinDouble ||
           base_action_score == BaseActionScore::TSpinDouble || base_action_score == BaseActionScore::TSpinTriple;
}

inline auto toBaseActionScore(size_t lines_cleared, TSpinType t_spin_type) -> std::optional<BaseActionScore> {
    if (t_spin_type == TSpinType::NotTSpin) {
        switch (lines_cleared) {
        case 0:
            return {};
        case 1:
            return BaseActionScore::Single;
        case 2:
            return BaseActionScore::Double;
        case 3:
            return BaseActionScore::Triple;
        case 4:
            return BaseActionScore::Tetris;
        default:
            break;
        }
    } else if (t_spin_type == TSpinType::NoWallKick) {
        switch (lines_cleared) {
        case 0:
            return BaseActionScore::TSpinZero;
        case 1:
            return BaseActionScore::TSpinSingle;
        case 2:
            return BaseActionScore::TSpinDouble;
        default:
            break;
        }
    } else if (t_spin_type == TSpinType::WallKick) {
        switch (lines_cleared) {
        case 0:
            return BaseActionScore::MiniTSpinZero;
        case 1:
            return BaseActionScore::MiniTSpinSingle;
        case 2:
            return BaseActionScore::MiniTSpinDouble;
        case 3:
            return BaseActionScore::TSpinTriple;
        default:
            break;
        }
    }
    assert(false);
    return {};
}

// TODO: perfect clear scoring
struct ScoreState {
    int current_score = 0;
    bool prev_b2b = false;
    bool curr_b2b = false;
    int combo_count = -1;

    static void printScoreType(bool b2b, BaseActionScore base_action_score, int combo_count) {
        if (b2b) {
            std::cout << "B2B ";
        }
        switch (base_action_score) {
        case BaseActionScore::Single:
            std::cout << "Single" << std::endl;
            break;
        case BaseActionScore::Double:
            std::cout << "Double" << std::endl;
            break;
        case BaseActionScore::Triple:
            std::cout << "Triple" << std::endl;
            break;
        case BaseActionScore::Tetris:
            std::cout << "Tetris" << std::endl;
            break;
        case BaseActionScore::MiniTSpinZero:
            std::cout << "MiniTSpinZero" << std::endl;
            break;
        case BaseActionScore::TSpinZero:
            std::cout << "TSpinZero" << std::endl;
            break;
        case BaseActionScore::MiniTSpinSingle:
            std::cout << "MiniTSpinSingle" << std::endl;
            break;
        case BaseActionScore::TSpinSingle:
            std::cout << "TSpinSingle" << std::endl;
            break;
        case BaseActionScore::MiniTSpinDouble:
            std::cout << "MiniTSpinDouble" << std::endl;
            break;
        case BaseActionScore::TSpinDouble:
            std::cout << "TSpinDouble" << std::endl;
            break;
        case BaseActionScore::TSpinTriple:
            std::cout << "TSpinTriple" << std::endl;
            break;
        }
        if (combo_count > 0) {
            std::cout << "combo x" << combo_count << std::endl;
        }
    }

    void score(BaseActionScore base_action_score, int level, int soft_drop, int hard_drop) {
        current_score += soft_drop + hard_drop * 2;
        combo_count++;
        int combo_score = combo_count > 0 ? 50 * combo_count * (level + 1) : 0;
        int curr_action_score = action_to_score.at(base_action_score) * (level + 1) + combo_score;
        curr_b2b = isDifficult(base_action_score);
        printScoreType(prev_b2b && curr_b2b, base_action_score, combo_count);
        current_score += (prev_b2b && curr_b2b) ? curr_action_score * 3 / 2 : curr_action_score;
        prev_b2b = curr_b2b;
    }

    void resetCombo() { combo_count = -1; }
};

} // namespace tetris