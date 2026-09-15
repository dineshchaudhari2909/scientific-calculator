// Backend configuration
// Automatically detect environment
if (window.location.hostname === 'localhost' || window.location.hostname === '127.0.0.1') {
    window.BACKEND_URL = "http://localhost:8080";
} else {
    // Production: use the deployed backend URL
    window.BACKEND_URL = "https://scientific-calculator-backend-9o2o.onrender.com";
}