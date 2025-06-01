```mermaid
erDiagram
HandleCollection {

}

HandleInterface {
}

Handle {
}

HandleCollection ||--|{ Handle : contains
Handle ||--|| Heater : contains

USBAdapter ||--|| HandleCollection : "add handle"

HandleInterface }|..|| HandleCollection : "find handle"

HandleInterface ||..|| Handle : "current handle"

USBAdapter ||..|| Handle : modify

MenuDisplay ||..|{ HandleInterface : "get current handle"
```
