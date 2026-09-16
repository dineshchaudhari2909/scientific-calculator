// Backend URL is loaded from config.js
// This file should be loaded before script.js in index.html

const display = document.getElementById("display");
const expression = document.getElementById("expression");
const error = document.getElementById("error");
const loading = document.getElementById("loading");

let currentInput = "0";
let currentOperator = null;
let previousInput = null;
let awaitingSecondOperand = false;
let isFloatMode = false;

// Update display
function updateDisplay() {
    display.textContent = currentInput;
    expression.textContent = previousInput && currentOperator
        ? `${previousInput} ${currentOperator}`
        : "";
}

// Clear calculator
function clearCalculator() {
    currentInput = "0";
    currentOperator = null;
    previousInput = null;
    awaitingSecondOperand = false;
    isFloatMode = false;
    error.textContent = "";
    updateDisplay();
}

// Backspace
function backspace() {
    if (currentInput.length > 1) {
        currentInput = currentInput.slice(0, -1);
        if (!currentInput.includes('.')) {
            isFloatMode = false;
        }
    } else {
        currentInput = "0";
    }
    updateDisplay();
}

// Handle number input
function handleNumber(value) {
    if (awaitingSecondOperand) {
        currentInput = value;
        awaitingSecondOperand = false;
        isFloatMode = false;
    } else {
        if (currentInput === "0" && value !== ".") {
            currentInput = value;
        } else {
            currentInput += value;
        }
    }
    updateDisplay();
}

// Handle decimal point
function handleDecimal() {
    if (!isFloatMode) {
        currentInput += ".";
        isFloatMode = true;
        updateDisplay();
    }
}

// Handle negation
function handleNegation() {
    if (currentInput !== "0") {
        if (currentInput.startsWith("-")) {
            currentInput = currentInput.slice(1);
        } else {
            currentInput = "-" + currentInput;
        }
        updateDisplay();
    }
}

// Handle operator
function handleOperator(operator) {
    if (previousInput && currentOperator && !awaitingSecondOperand) {
        calculate();
    }

    previousInput = currentInput;
    currentOperator = operator;
    awaitingSecondOperand = true;
    isFloatMode = false;
    updateDisplay();
}

// Check if input is integer
function isInteger(str) {
    return /^-?\d+$/.test(str);
}

// Send to C++ backend for integer operations
async function sendToBackend(a, b, operation) {
    try {
        const response = await fetch(BACKEND_URL + "/calculate", {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                a: a,
                b: b,
                operation: operation
            })
        });

        const data = await response.json();

        if (!response.ok) {
            throw new Error(data.error || "Calculation failed");
        }

        return data.result;
    } catch (err) {
        throw new Error("Unable to connect to C++ backend: " + err.message);
    }
}

// Calculate result
async function calculate() {
    if (!previousInput || !currentOperator) {
        return;
    }

    error.textContent = "";
    loading.classList.remove("hidden");

    const a = previousInput;
    const b = currentInput;
    const op = currentOperator;

    try {
        let result;

        // Division and modulo are always handled client-side for floating-point results
        if (op === "/" || op === "÷" || op === "%") {
            const numA = parseFloat(a);
            const numB = parseFloat(b);

            if (op === "/" || op === "÷") {
                if (numB === 0) {
                    throw new Error("Cannot divide by zero");
                }
                result = (numA / numB).toString();
            } else {
                // Proper floating-point modulo
                if (numB === 0) {
                    throw new Error("Cannot modulo by zero");
                }
                result = ((numA % numB) + numB) % numB; // Fix for negative numbers
                result = result.toString();
            }
        }
        // Integer operations go to C++ backend for arbitrary-precision
        else if (isInteger(a) && isInteger(b) && ["+", "-", "*", "^", "gcd", "lcm"].includes(op)) {
            const backendOp = op === "*" ? "*" : op === "×" ? "*" : op === "−" ? "-" : op;
            result = await sendToBackend(a, b, backendOp);
        } else {
            // Floating-point operations handled client-side
            const numA = parseFloat(a);
            const numB = parseFloat(b);

            switch (op) {
                case "+":
                    result = (numA + numB).toString();
                    break;
                case "-":
                case "−":
                    result = (numA - numB).toString();
                    break;
                case "*":
                case "×":
                    result = (numA * numB).toString();
                    break;
                case "^":
                    result = Math.pow(numA, numB).toString();
                    break;
                default:
                    throw new Error("Unsupported operation");
            }
        }

        currentInput = result;
        previousInput = null;
        currentOperator = null;
        awaitingSecondOperand = false;
        isFloatMode = result.includes(".");

        updateDisplay();
    } catch (err) {
        error.textContent = err.message;
    } finally {
        loading.classList.add("hidden");
    }
}

// Handle scientific functions
async function handleScientific(action) {
    error.textContent = "";
    loading.classList.remove("hidden");

    const a = currentInput;

    try {
        let result;

        switch (action) {
            case "sin":
                result = Math.sin(parseFloat(a)).toString();
                break;
            case "cos":
                result = Math.cos(parseFloat(a)).toString();
                break;
            case "tan":
                result = Math.tan(parseFloat(a)).toString();
                break;
            case "log":
                result = Math.log10(parseFloat(a)).toString();
                break;
            case "ln":
                result = Math.log(parseFloat(a)).toString();
                break;
            case "sqrt":
                if (parseFloat(a) < 0) {
                    throw new Error("Cannot calculate square root of negative number");
                }
                result = Math.sqrt(parseFloat(a)).toString();
                break;
            case "factorial":
                if (isInteger(a) && parseInt(a) >= 0) {
                    result = await sendToBackend(a, "", "!");
                } else {
                    throw new Error("Factorial requires non-negative integer");
                }
                break;
            case "gcd":
            case "lcm":
                // These require two operands, set up operator mode
                handleOperator(action);
                loading.classList.add("hidden");
                return;
            case "power":
                handleOperator("^");
                loading.classList.add("hidden");
                return;
            default:
                throw new Error("Unknown function");
        }

        currentInput = result;
        isFloatMode = result.includes(".");
        updateDisplay();
    } catch (err) {
        error.textContent = err.message;
    } finally {
        loading.classList.add("hidden");
    }
}

// Button click handlers
document.querySelectorAll('.btn').forEach(button => {
    button.addEventListener('click', () => {
        const value = button.dataset.value;
        const action = button.dataset.action;

        if (value) {
            if (value === '.') {
                handleDecimal();
            } else if (value === '-') {
                if (currentInput === "0") {
                    handleNegation();
                } else {
                    handleOperator(value);
                }
            } else if (['+', '*', '/', '%'].includes(value)) {
                handleOperator(value);
            } else {
                handleNumber(value);
            }
        } else if (action) {
            switch (action) {
                case 'clear':
                    clearCalculator();
                    break;
                case 'backspace':
                    backspace();
                    break;
                case 'calculate':
                    calculate();
                    break;
                case 'negate':
                    handleNegation();
                    break;
                default:
                    handleScientific(action);
            }
        }
    });
});

// Keyboard support
document.addEventListener('keydown', (e) => {
    const key = e.key;

    if (key >= '0' && key <= '9') {
        handleNumber(key);
    } else if (key === '.') {
        handleDecimal();
    } else if (key === '+' || key === '-' || key === '*' || key === '/' || key === '%') {
        handleOperator(key);
    } else if (key === 'Enter' || key === '=') {
        e.preventDefault();
        calculate();
    } else if (key === 'Escape' || key === 'c' || key === 'C') {
        clearCalculator();
    } else if (key === 'Backspace') {
        backspace();
    }
});

// Initialize
updateDisplay();