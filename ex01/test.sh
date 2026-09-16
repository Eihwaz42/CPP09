#!/bin/bash

echo "========== SUBJECT TESTS =========="

echo 'Test: "8 9 * 9 - 9 - 9 - 4 - 1 +" | Expected: 42'
./RPN "8 9 * 9 - 9 - 9 - 4 - 1 +"

echo 'Test: "7 7 * 7 -" | Expected: 42'
./RPN "7 7 * 7 -"

echo 'Test: "1 2 * 2 / 2 * 2 4 - +" | Expected: 0'
./RPN "1 2 * 2 / 2 * 2 4 - +"

echo 'Test: "(1 + 1)" | Expected: Error'
./RPN "(1 + 1)"


echo
echo "========== BASIC OPERATIONS =========="

echo 'Test: "3 4 +" | Expected: 7'
./RPN "3 4 +"

echo 'Test: "8 3 -" | Expected: 5'
./RPN "8 3 -"

echo 'Test: "8 2 /" | Expected: 4'
./RPN "8 2 /"

echo 'Test: "9 9 *" | Expected: 81'
./RPN "9 9 *"

echo 'Test: "8 3 - 2 *" | Expected: 10'
./RPN "8 3 - 2 *"


echo
echo "========== ERROR TESTS =========="

echo 'Test: "5 0 /" | Expected: Error'
./RPN "5 0 /"

echo 'Test: "1 +" | Expected: Error'
./RPN "1 +"

echo 'Test: "1 2" | Expected: Error'
./RPN "1 2"

echo 'Test: "42" | Expected: Error'
./RPN "42"

echo 'Test: "1.5 2 +" | Expected: Error'
./RPN "1.5 2 +"

echo 'Test: "1 2 truc" | Expected: Error'
./RPN "1 2 truc"

echo 'Test: empty expression | Expected: Error'
./RPN ""

echo 'Test: no argument | Expected: Error'
./RPN