extract_parm() {
    local param=$1
    local value=$(grep -E "^$param " | cut -d= -f2)
    echo $value
}