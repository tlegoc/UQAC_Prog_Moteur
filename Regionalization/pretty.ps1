$sources = @("examples","include","lib","src")
$sources | ForEach-Object {
  Get-ChildItem $_ -Include @("*.h","*.cpp","*.inl") -Recurse | ForEach-Object {
    & "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\Llvm\bin\clang-format.exe" -i $_.FullName
  }
}
