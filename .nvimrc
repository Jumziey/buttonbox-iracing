let s:projectRoot=expand("%:p")

"let g:neomake_cpp_clangtidy_args = ["-extra-arg=-I".s:projectRoot] ", -extra-arg=-I.\third_party\glfw\include -extra-arg=-std=c++17]
"let g:neomake_cpp_enabled_makers = ["clangtidy"]
let g:neomake_cpp_enabled_makers = ["cppcheck"]
