#include <algorithm>
#include <array>
#include <string_view>

#ifdef _WIN32
#include <Windows.h>
#else
#include <SDL2/SDL.h>
#endif

#include "imgui/imgui.h"

#include "InputUtil.h"
#include "SDK/Platform.h"

struct Key {
    template <std::size_t N>
    constexpr Key(const char(&name)[N], int code) : name{ name }, code{ code } {  }

    std::string_view name;
    int code;
};

// indices must match KeyBind::KeyCode enum, and has to be sorted alphabetically
constexpr auto keyMap = std::to_array<Key>({
    { "0", WIN32_LINUX('0', SDL_SCANCODE_0) },
    { "1", WIN32_LINUX('1', SDL_SCANCODE_1) },
    { "2", WIN32_LINUX('2', SDL_SCANCODE_2) },
    { "3", WIN32_LINUX('3', SDL_SCANCODE_3) },
    { "4", WIN32_LINUX('4', SDL_SCANCODE_4) },
    { "5", WIN32_LINUX('5', SDL_SCANCODE_5) },
    { "6", WIN32_LINUX('6', SDL_SCANCODE_6) },
    { "7", WIN32_LINUX('7', SDL_SCANCODE_7) },
    { "8", WIN32_LINUX('8', SDL_SCANCODE_8) },
    { "9", WIN32_LINUX('9', SDL_SCANCODE_9) },
    { "A", WIN32_LINUX('A', SDL_SCANCODE_A) },
    { "B", WIN32_LINUX('B', SDL_SCANCODE_B) },
    { "C", WIN32_LINUX('C', SDL_SCANCODE_C) },
    { "D", WIN32_LINUX('D', SDL_SCANCODE_D) },
    { "E", WIN32_LINUX('E', SDL_SCANCODE_E) },
    { "F", WIN32_LINUX('F', SDL_SCANCODE_F) },
    { "G", WIN32_LINUX('G', SDL_SCANCODE_G) },
    { "H", WIN32_LINUX('H', SDL_SCANCODE_H) },
    { "I", WIN32_LINUX('I', SDL_SCANCODE_I) },
    { "INSERT", WIN32_LINUX(VK_INSERT, SDL_SCANCODE_INSERT) },
    { "J", WIN32_LINUX('J', SDL_SCANCODE_J) },
    { "K", WIN32_LINUX('K', SDL_SCANCODE_K) },
    { "L", WIN32_LINUX('L', SDL_SCANCODE_L) },
    { "M", WIN32_LINUX('M', SDL_SCANCODE_M) },
    { "MOUSE1", 0 },
    { "MOUSE2", 1 },
    { "MOUSE3", 2 },
    { "MOUSE4", 3 },
    { "MOUSE5", 4 },
    { "MOUSEWHEEL_DOWN", 0 },
    { "MOUSEWHEEL_UP", 0 },
    { "N", WIN32_LINUX('N', SDL_SCANCODE_N) },
    { "NONE", 0 },
    { "O", WIN32_LINUX('O', SDL_SCANCODE_O) },
    { "P", WIN32_LINUX('P', SDL_SCANCODE_P) },
    { "Q", WIN32_LINUX('Q', SDL_SCANCODE_Q) },
    { "R", WIN32_LINUX('R', SDL_SCANCODE_R) },
    { "S", WIN32_LINUX('S', SDL_SCANCODE_S) },
    { "T", WIN32_LINUX('T', SDL_SCANCODE_T) },
    { "U", WIN32_LINUX('U', SDL_SCANCODE_U) },
    { "V", WIN32_LINUX('V', SDL_SCANCODE_V) },
    { "W", WIN32_LINUX('W', SDL_SCANCODE_W) },
    { "X", WIN32_LINUX('X', SDL_SCANCODE_X) },
    { "Y", WIN32_LINUX('Y', SDL_SCANCODE_Y) },
    { "Z", WIN32_LINUX('Z', SDL_SCANCODE_X) },
});

KeyBind::KeyBind(KeyCode keyCode) noexcept
{
    this->keyCode = static_cast<std::size_t>(keyCode) < keyMap.size() ? keyCode : KeyCode::NONE;
}

KeyBind::KeyBind(const char* keyName) noexcept
{
    auto it = std::lower_bound(keyMap.begin(), keyMap.end(), keyName, [](const Key& key, const char* keyName) { return key.name < keyName; });
    if (it != keyMap.end() && it->name == keyName)
        keyCode = static_cast<KeyCode>(std::distance(keyMap.begin(), it));
    else
        keyCode = KeyCode::NONE;
}

const char* KeyBind::toString() const noexcept
{
    return keyMap[static_cast<std::size_t>(keyCode) < keyMap.size() ? keyCode : KeyCode::NONE].name.data();
}

bool KeyBind::isPressed() const noexcept
{
    if (keyCode == KeyCode::NONE)
        return false;

    if (keyCode == KeyCode::MOUSEWHEEL_DOWN)
        return ImGui::GetIO().MouseWheel < 0.0f;

    if (keyCode == KeyCode::MOUSEWHEEL_UP)
        return ImGui::GetIO().MouseWheel > 0.0f;

    if (keyCode >= KeyCode::MOUSE1 && keyCode <= KeyCode::MOUSE5)
        return ImGui::IsMouseClicked(keyMap[keyCode].code);

    return static_cast<std::size_t>(keyCode) < keyMap.size() && ImGui::IsKeyPressed(keyMap[keyCode].code, false);
}

bool KeyBind::isDown() const noexcept
{
    if (keyCode == KeyCode::NONE)
        return false;

    if (keyCode == KeyCode::MOUSEWHEEL_DOWN)
        return ImGui::GetIO().MouseWheel < 0.0f;

    if (keyCode == KeyCode::MOUSEWHEEL_UP)
        return ImGui::GetIO().MouseWheel > 0.0f;

    if (keyCode >= KeyCode::MOUSE1 && keyCode <= KeyCode::MOUSE5)
        return ImGui::IsMouseDown(keyMap[keyCode].code);

    return static_cast<std::size_t>(keyCode) < keyMap.size() && ImGui::IsKeyDown(keyMap[keyCode].code);
}

void KeyBind::setToPressedKey() noexcept
{
    if (ImGui::IsKeyPressed(ImGui::GetIO().KeyMap[ImGuiKey_Escape])) {
        keyCode = KeyCode::NONE;
    } else if (ImGui::GetIO().MouseWheel < 0.0f) {
        keyCode = KeyCode::MOUSEWHEEL_DOWN;
    } else if (ImGui::GetIO().MouseWheel > 0.0f) {
        keyCode = KeyCode::MOUSEWHEEL_UP;
    } else {
        for (int i = 0; i < IM_ARRAYSIZE(ImGui::GetIO().MouseDown); ++i) {
            if (ImGui::IsMouseClicked(i)) {
                keyCode = KeyCode(KeyCode::MOUSE1 + i);
                return;
            }
        }

        for (int i = 0; i < IM_ARRAYSIZE(ImGui::GetIO().KeysDown); ++i) {
            if (ImGui::IsKeyPressed(i)) {
                auto it = std::find_if(keyMap.begin(), keyMap.end(), [i](const Key& key) { return key.code == i; });
                if (it != keyMap.end()) {
                    keyCode = static_cast<KeyCode>(std::distance(keyMap.begin(), it));
                    return;
                }
            }
        }
    }
}