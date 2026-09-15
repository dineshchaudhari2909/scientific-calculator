FROM gcc:11

WORKDIR /app

COPY arithmetic.h .
COPY arithmetic.cpp .
COPY main.cpp .

# Build for Linux (no Windows-specific libraries needed)
RUN g++ -std=c++17 -O2 main.cpp arithmetic.cpp -o calculator

# Render will set PORT environment variable automatically
# Our application reads it with std::getenv("PORT")

EXPOSE 8080

# Run the calculator
CMD ["./calculator"]