# Data Structures and Algorithms Project 2025

## Overview

The purpose of this project is to put into practice the C++ programming language and the
lessons learned in the Data Structures and Algorithms course.

The project is a server application designed to generate health parameter graphs based on user input. It includes a web interface for user interaction and utilizes several technologies to handle HTTP requests, generate PNG images, and store user data.

## Tech Stack

### Frontend (Web)

- **HTML/JavaScript**:
  - Couple of JS functions to send JSON from <form>.

### Backend (C++)

- **HTTP Server**: [Crow](https://github.com/CrowCpp/Crow)
  - Crow is used to process HTTP requests, serve HTML content to the web UI, manage user sessions with cookies, and store user IDs in the database.

- **PNG Encoding**: [LodePNG](https://github.com/lvandeve/lodepng)
  - LodePNG is used to encode graphs, that are renderer by writing pixels directly into a `std::vector`.

- **Database**: [SQLite](https://www.sqlite.org/)
  - SQLite is used to store user input data securely.

## Getting Started

### Installation

1. Clone the repository:
    ```sh
    git clone git@github.com:grem-newfield/Algorithm_Project_SlimMachines.git
    cd Algorithm_Project_SlimMachines
    ```

2. Build the project:

    ```sh
    just build # LINUX ONLY
    # OR
    just docker-build # to run locally.
    ```

3. Run the server:
    ```sh
    ./slim_machines_app
    # OR
    just docker-run
    ```

## Usage

1. Open your web browser and navigate to `http://localhost:8080`.
2. Input your health parameters as required.
3. View the generated graphs on the web interface.

## License

This project is provided "AS IS" without any warranty of any kind, express or implied. Use at your own risk. No license is granted.

## People

- **German Novopoltsev** - [@grem-newfield](https://github.com/grem-newfield)
- **Sabbir Raihan Shishir** - [@srshishir04](https://github.com/srshishir04)
- **Vitalii Virronen** - [@VitaliiVIP](https://github.com/VitaliiVIP)

## Acknowledgments

- [Crow](https://github.com/CrowCpp/Crow)
- [LodePNG](https://github.com/lvandeve/lodepng)
- [SQLite](https://www.sqlite.org/)
