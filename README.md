# Pokedex Document

## How to compile this project

```bash
just # List recipes
just build model # Use nix to build model, artifacts store in ./result/
just compile model # Compile project locally for debug build
just develop simulator # Entering development shell for simulator
just cfg=zve32x compile model # Use zve32x.toml to debug compile the project
just guidance #Compile the PDF
```
