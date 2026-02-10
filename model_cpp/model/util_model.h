#pragma once

#include <optional>
#include <cstdint>
#include <cassert>

#include "util_arith.h"
#include "util_inst.h"

template<typename Model>
struct ModelHelper {
    static constexpr PrivMode LEAST_PRIV_MODE = Model::U_MODE ? PrivMode::U : PrivMode::M;

    static bool is_valid_priv_mode(PrivMode mode) {
        switch (mode) {
            case PrivMode::U: return Model::U_MODE;
            case PrivMode::S: return Model::S_MODE;
            case PrivMode::M: return Model::M_MODE;
            default: return false;
        }
    }

    static bool is_pc_aligned(uint32_t x) {
        if (Model::EXT_C) {
            return (x & 1) == 0;
        } else {
            return (x & 3) == 0;
        }
    }

    bool resolve_frm(FrmField static_frm, RoundingMode* rm_out) const {
        if (static_frm.value <= 4) {
            *rm_out = RoundingMode(static_frm.value);
            return true;
        }
        
        if (static_frm.value == 7) {
            uint8_t dyn_frm = static_cast<const Model*>(this)->_get_frm();
            if (dyn_frm <= 4) {
                *rm_out = RoundingMode(dyn_frm);
                return true;
            }
        }

        return false;
    }
};
