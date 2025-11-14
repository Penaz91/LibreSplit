Contributing
============

Generating the developer documentation
--------------------------------------

The code is commented in a way that allows the creation of developer documentation using [Doxygen](https://doxygen.nl).

First of all, you'll need to install Doxygen:

- Debian-based systems

    ```sh
    sudo apt update
    sudo apt install doxygen
    ```

- Arch-based systems

    ```sh
    sudo pacman -Sy
    sudo pacman -S doxygen
    ```

Then you can generate the documentation by running:

```sh
doxygen Doxyfile
```

From the root of the project. Doxygen will automatically generate HTML documentation and place it under the `developer_docs/html/` subdirectory.

To give it a quick read, you can use your favourite browser directly or spin up a quick local server using Python:

```sh
python -m http.server --bind 127.0.0.1 5000
```

And then navigating to `localhost:5000`.
