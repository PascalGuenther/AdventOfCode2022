export interface WasmCallbacks {
    reiceivePuzzleSolution: (day: number, part: number, solution?: string) => void;
    debug?: (code: number) => void;
}
