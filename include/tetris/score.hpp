namespace tetris {

enum class BaseActionScore : int {
    Single = 100,
    Double = 300,
    Triple = 500,
    Tetris = 800,
    MiniTSpinZero = 100,
    TSpinZero = 400,
    MiniTSpinSingle = 200,
    TSpinSingle = 800,
    MiniTSpinDouble = 400,
    TSpinDouble = 1200,
    TSpinTriple = 1600
};

inline auto isDifficult(BaseActionScore base_action_score) -> bool {
    return base_action_score == BaseActionScore::Tetris || base_action_score == BaseActionScore::MiniTSpinSingle ||
           base_action_score == BaseActionScore::TSpinSingle || base_action_score == BaseActionScore::MiniTSpinDouble ||
           base_action_score == BaseActionScore::TSpinDouble || base_action_score == BaseActionScore::TSpinTriple;
}

struct ScoreState {
    int current_score = 0;
    bool prev_b2b = false;
    bool curr_b2b = false;
    int combo_count = -1;

    void score(BaseActionScore base_action_score, int level, int soft_drop, int hard_drop) {
        combo_count++;
        if (combo_count > 0) {
            current_score += 50 * combo_count * level;
        }
        curr_b2b = isDifficult(base_action_score);
        if (prev_b2b && curr_b2b) {
            current_score += static_cast<int>(base_action_score) * level * 3 / 2;
        } else {
            current_score += static_cast<int>(base_action_score) * level + soft_drop + hard_drop * 2;
            prev_b2b = curr_b2b;
        }
    }

    void resetCombo() { combo_count = -1; }
};

} // namespace tetris