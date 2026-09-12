# Scientific Calculator - Production Deployment Guide

## Overview
This Scientific Calculator uses a C++ backend with arbitrary-precision integer arithmetic and a modern HTML/CSS/JavaScript frontend. The backend is designed for deployment on Render using Docker containers.

## Architecture
```
GitHub → Render → Linux GCC → C++ Backend → Frontend
```

## Files Changed

### Backend (Cross-Platform)
- **backend/main.cpp**: Added cross-platform socket support (Windows Winsock + Linux POSIX)
- **backend/Dockerfile**: Docker configuration for Linux build
- **backend/arithmetic.cpp**: Platform-independent (no changes needed)
- **backend/arithmetic.h**: Platform-independent (no changes needed)

### Frontend
- **frontend/config.js**: New configuration file for backend URL
- **frontend/script.js**: Updated to use configurable backend URL
- **frontend/index.html**: Added config.js script loading

### Configuration
- **render.yaml**: Render deployment configuration
- **Dockerfile**: Container build configuration

## Render Build Command
```bash
g++ -std=c++17 -O2 main.cpp arithmetic.cpp -o calculator
```

## Render Start Command
```bash
./calculator
```

## Environment Variables
- **PORT**: Server port (default: 8080, set by Render automatically)

## Deployment Steps

### 1. Push to GitHub
```bash
git add .
git commit -m "Prepare for Render deployment"
git push origin main
```

### 2. Connect Repository to Render
1. Go to [Render Dashboard](https://dashboard.render.com)
2. Click "New +" → "Web Service"
3. Connect your GitHub repository
4. Select the repository

### 3. Configure Render Service
- **Name**: scientific-calculator-backend
- **Environment**: Docker
- **Docker Context**: ./backend
- **Dockerfile Path**: ./backend/Dockerfile
- **Region**: Oregon (or your preferred region)
- **Branch**: main
- **Plan**: Free

### 4. Set Environment Variables
- **PORT**: 8080 (Render will set this automatically)

### 5. Deploy
Click "Create Web Service" and Render will build and deploy your service.

## Local Testing Steps

### Windows Development
```bash
cd backend
g++ -std=c++17 -O2 -static -static-libgcc -static-libstdc++ main.cpp arithmetic.cpp -o calculator -lws2_32
calculator.exe
```

### Linux Development
```bash
cd backend
g++ -std=c++17 -O2 main.cpp arithmetic.cpp -o calculator
./calculator
```

### Docker Build (Linux-like environment)
```bash
cd backend
docker build -t scientific-calculator .
docker run -p 8080:8080 scientific-calculator
```

### Test Backend
```bash
# Health check
curl http://localhost:8080/health

# Test calculation
curl -X POST http://localhost:8080/calculate \
  -H "Content-Type: application/json" \
  -d '{"a":"123","b":"456","operation":"+"}'
```

### Test Frontend
```bash
cd frontend
python -m http.server 5500
# Open http://localhost:5500 in browser
```

## Production API URL Configuration

### After Deployment
1. Find your Render service URL (e.g., `https://scientific-calculator-backend.onrender.com`)
2. Update `frontend/config.js`:
```javascript
window.BACKEND_URL = "https://scientific-calculator-backend.onrender.com";
```

### Frontend Deployment Options
Since the frontend is static HTML/CSS/JS, you can deploy it to:
- **Render Static Site**: Deploy frontend separately
- **Netlify**: Drag and drop frontend folder
- **GitHub Pages**: Use gh-pages branch
- **Vercel**: Connect to your repository

### Example: Frontend on Netlify
1. Go to [Netlify](https://netlify.com)
2. Drag and drop the `frontend/` folder
3. Edit `frontend/config.js` with your production backend URL
4. The site will be live instantly

## Key Features

### Cross-Platform Backend
- **Windows**: Uses Winsock2 for networking
- **Linux**: Uses POSIX sockets
- **Conditional compilation**: `#ifdef _WIN32` for platform-specific code
- **Environment variables**: Reads PORT from environment
- **Bind address**: Listens on 0.0.0.0 for production compatibility

### Frontend Configuration
- **Configurable backend URL**: Set in `frontend/config.js`
- **No hard-coded URLs**: No localhost or 127.0.0.1 in production
- **Easy switching**: Change one line for different environments

### Build Compatibility
- **GCC 11.4.0**: Tested on Windows with MinGW
- **C++17**: Modern C++ standard for cross-platform compatibility
- **No external dependencies**: Pure C++ standard library
- **Docker**: Ensures consistent Linux build environment

## Important Notes

### Windows Development Support Preserved
- The Windows build still works with `-lws2_32` flag
- Windows-specific code wrapped in `#ifdef _WIN32`
- No breaking changes to local development workflow

### Production-Specific Changes
- Backend binds to `0.0.0.0` instead of `localhost`
- Port read from `PORT` environment variable
- Docker ensures Linux build consistency
- Frontend uses configurable backend URL

### No Arbitrary-Precision Libraries
- Uses manual string/vector<int> implementation
- No BigInt, Boost.Multiprecision, or GMP
- Full arbitrary-precision support on both platforms

## Troubleshooting

### Backend won't start on Render
- Check Render logs for compilation errors
- Ensure Dockerfile is in the correct location
- Verify PORT environment variable is set

### Frontend can't connect to backend
- Update `frontend/config.js` with correct backend URL
- Check if backend is running (health check)
- Verify CORS is enabled (should be by default)

### Build fails locally
- Ensure GCC 11.4.0 or later is installed
- On Windows, use `-lws2_32` for Windows networking
- On Linux, no extra flags needed

## Monitoring
- Render provides built-in monitoring
- Check health endpoint: `https://your-service.onrender.com/health`
- Monitor logs in Render dashboard

## Security Considerations
- CORS enabled for frontend access
- Request size limits prevent memory issues
- Input validation on both frontend and backend
- No secrets or API keys needed