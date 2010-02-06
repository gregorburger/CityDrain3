BEGIN { rain = 0.0 }
{rain += $3}
END {print rain}
