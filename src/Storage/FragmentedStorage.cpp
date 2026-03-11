// //
// // Created by bobi on 7. 03. 26.
// //
//
// #include "FragmentedStorage.h"
//
// using namespace Utils;
//
// template <typename T, size_t TSize>
// requires std::is_base_of_v<Fragment, T>
// void FragmentedStorage<T, TSize>::fillBytesPatched(uint32_t start, uint32_t end)
// {
//     std::fill(m_bytesPatched.begin() + start, m_bytesPatched.begin() + end, m_fragmentsNum);
//     m_fragmentsNum++;
// }
//
// template <typename T, size_t TSize>
// requires std::is_base_of_v<Fragment, T>
// uint32_t FragmentedStorage<T, TSize>::numFragmentsBefore(uint32_t start) {
//     uint32_t counter = 0U;
//     uint32_t prev = 0U;
//
//     for (uint32_t i = 0; i < start; i++)
//     {
//         auto v = m_bytesPatched[i];
//         if (v != 0 && prev != v) {
//             counter++;
//             prev = v;
//         }
//     }
//
//     return counter;
// }
//
// template <typename T, size_t TSize>
// requires std::is_base_of_v<Fragment, T>
// std::optional<uint32_t> FragmentedStorage<T, TSize>::findFirstEmpty(uint32_t size) {
//     for (int i = 0; i < TSize - size; i++) {
//         bool isEmpty = isFree(i);
//         if (isEmpty) {
//             uint32_t counter = 0;
//             for (; counter < size; counter++) {
//                 isEmpty = isFree(counter + i);
//                 if (!isEmpty) {
//                     break;
//                 }
//             }
//
//             if (counter == size) {
//                 return i;
//             }
//         }
//     }
//
//     return std::nullopt;
// };