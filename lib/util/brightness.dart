import 'package:flutter/material.dart';

bool isDarkMode(BuildContext context){
  return Theme.of(context).brightness == Brightness.dark;
}

T byBrightness<T>(BuildContext context, {required T light, required  T dark}){
  return Theme.of(context).brightness == Brightness.light? light: dark;
}
